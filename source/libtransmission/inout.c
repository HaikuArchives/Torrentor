/*
 * This file Copyright (C) Mnemosyne LLC
 *
 * This file is licensed by the GPL version 2. Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: inout.c 11811 2011-02-02 06:06:09Z jordan $
 */

#ifdef HAVE_LSEEK64
 #define _LARGEFILE64_SOURCE
#endif

#include <assert.h>
#include <errno.h>
#include <stdlib.h> /* realloc */
#include <string.h> /* memcmp */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <openssl/sha.h>

#include "transmission.h"
#include "cache.h"
#include "crypto.h"
#include "fdlimit.h"
#include "inout.h"
#include "peer-common.h" /* MAX_BLOCK_SIZE */
#include "platform.h"
#include "stats.h"
#include "torrent.h"
#include "utils.h"

/****
*****  Low-level IO functions
****/

#ifdef WIN32
 #if defined(read)
  #undef read
 #endif
 #define read  _read

 #if defined(write)
  #undef write
 #endif
 #define write _write
#endif

enum { TR_IO_READ, TR_IO_PREFETCH,
       /* Any operations that require write access must follow TR_IO_WRITE. */
       TR_IO_WRITE
};

/* returns 0 on success, or an errno on failure */
static int
readOrWriteBytes( tr_session       * session,
                  tr_torrent       * tor,
                  int                ioMode,
                  tr_file_index_t    fileIndex,
                  uint64_t           fileOffset,
                  void             * buf,
                  size_t             buflen )
{
    const tr_info * info = &tor->info;
    const tr_file * file = &info->files[fileIndex];

    int             fd = -1;
    int             err = 0;
    const tr_bool doWrite = ioMode >= TR_IO_WRITE;

//if( doWrite )
//    fprintf( stderr, "in file %s at offset %zu, writing %zu bytes; file length is %zu\n", file->name, (size_t)fileOffset, buflen, (size_t)file->length );

    assert( fileIndex < info->fileCount );
    assert( !file->length || ( fileOffset < file->length ) );
    assert( fileOffset + buflen <= file->length );

    if( !file->length )
        return 0;

    fd = tr_fdFileGetCached( session, tr_torrentId( tor ), fileIndex, doWrite );

    if( fd < 0 )
    {
        /* the fd cache doesn't have this file...
         * we'll need to open it and maybe create it */
        char * subpath;
        const char * base;
        tr_bool fileExists;
        tr_preallocation_mode preallocationMode;

        fileExists = tr_torrentFindFile2( tor, fileIndex, &base, &subpath );

        if( !fileExists )
        {
            base = tr_torrentGetCurrentDir( tor );

            if( tr_sessionIsIncompleteFileNamingEnabled( tor->session ) )
                subpath = tr_torrentBuildPartial( tor, fileIndex );
            else
                subpath = tr_strdup( file->name );
        }

        if( ( file->dnd ) || ( ioMode < TR_IO_WRITE ) )
            preallocationMode = TR_PREALLOCATE_NONE;
        else
            preallocationMode = tor->session->preallocationMode;

        if( ( ioMode < TR_IO_WRITE ) && !fileExists ) /* does file exist? */
        {
            err = ENOENT;
        }
        else
        {
            char * filename = tr_buildPath( base, subpath, NULL );

            if( ( fd = tr_fdFileCheckout( session, tor->uniqueId, fileIndex, filename,
                                          doWrite, preallocationMode, file->length ) ) < 0 )
            {
                err = errno;
                tr_torerr( tor, "tr_fdFileCheckout failed for \"%s\": %s", filename, tr_strerror( err ) );
            }

            tr_free( filename );
        }

        if( doWrite && !err )
            tr_statsFileCreated( tor->session );

        tr_free( subpath );
    }

    /* check that the file corresponding to 'fd' still exists */
    if( fd >= 0 )
    {
        struct stat sb;

        if( !fstat( fd, &sb ) && sb.st_nlink < 1 )
        {
            tr_torrentSetLocalError( tor, "Please Verify Local Data! A file disappeared: \"%s\"", file->name );
            err = ENOENT;
        }
    }

    if( !err )
    {
        if( ioMode == TR_IO_READ ) {
            const int rc = tr_pread( fd, buf, buflen, fileOffset );
            if( rc < 0 ) {
                err = errno;
                tr_torerr( tor, "read failed for \"%s\": %s",
                           file->name, tr_strerror( err ) );
            }
        } else if( ioMode == TR_IO_PREFETCH ) {
            const int rc = tr_prefetch( fd, fileOffset, buflen );
            if( rc < 0 ) {
                /* (don't set "err" here... it's okay for prefetch to fail) */
                tr_tordbg( tor, "prefetch failed for \"%s\": %s",
                           file->name, tr_strerror( errno ) );
            }
        } else if( ioMode == TR_IO_WRITE ) {
            const int rc = tr_pwrite( fd, buf, buflen, fileOffset );
            if( rc < 0 ) {
                err = errno;
                tr_torerr( tor, "write failed for \"%s\": %s",
                           file->name, tr_strerror( err ) );
            }
        } else {
            abort();
        }
    }

    return err;
}

static int
compareOffsetToFile( const void * a, const void * b )
{
    const uint64_t  offset = *(const uint64_t*)a;
    const tr_file * file = b;

    if( offset < file->offset ) return -1;
    if( offset >= file->offset + file->length ) return 1;
    return 0;
}

void
tr_ioFindFileLocation( const tr_torrent * tor,
                       tr_piece_index_t   pieceIndex,
                       uint32_t           pieceOffset,
                       tr_file_index_t  * fileIndex,
                       uint64_t         * fileOffset )
{
    const uint64_t  offset = tr_pieceOffset( tor, pieceIndex, pieceOffset, 0 );
    const tr_file * file;

    assert( tr_isTorrent( tor ) );
    assert( offset < tor->info.totalSize );

    file = bsearch( &offset,
                    tor->info.files, tor->info.fileCount, sizeof( tr_file ),
                    compareOffsetToFile );

    assert( file != NULL );

    *fileIndex = file - tor->info.files;
    *fileOffset = offset - file->offset;

    assert( *fileIndex < tor->info.fileCount );
    assert( *fileOffset < file->length );
    assert( tor->info.files[*fileIndex].offset + *fileOffset == offset );
}

/* returns 0 on success, or an errno on failure */
static int
readOrWritePiece( tr_torrent       * tor,
                  int                ioMode,
                  tr_piece_index_t   pieceIndex,
                  uint32_t           pieceOffset,
                  uint8_t          * buf,
                  size_t             buflen )
{
    int             err = 0;
    tr_file_index_t fileIndex;
    uint64_t        fileOffset;
    const tr_info * info = &tor->info;

    if( pieceIndex >= tor->info.pieceCount )
        return EINVAL;
    //if( pieceOffset + buflen > tr_torPieceCountBytes( tor, pieceIndex ) )
    //    return EINVAL;

    tr_ioFindFileLocation( tor, pieceIndex, pieceOffset,
                           &fileIndex, &fileOffset );

    while( buflen && !err )
    {
        const tr_file * file = &info->files[fileIndex];
        const uint64_t bytesThisPass = MIN( buflen, file->length - fileOffset );

        err = readOrWriteBytes( tor->session, tor, ioMode, fileIndex, fileOffset, buf, bytesThisPass );
        buf += bytesThisPass;
        buflen -= bytesThisPass;
//fprintf( stderr, "++fileIndex to %d\n", (int)fileIndex );
        ++fileIndex;
        fileOffset = 0;

        if( ( err != 0 ) && (ioMode == TR_IO_WRITE ) && ( tor->error != TR_STAT_LOCAL_ERROR ) )
        {
            char * path = tr_buildPath( tor->downloadDir, file->name, NULL );
            tr_torrentSetLocalError( tor, "%s (%s)", tr_strerror( err ), path );
            tr_free( path );
        }
    }

    return err;
}

int
tr_ioRead( tr_torrent       * tor,
           tr_piece_index_t   pieceIndex,
           uint32_t           begin,
           uint32_t           len,
           uint8_t          * buf )
{
    return readOrWritePiece( tor, TR_IO_READ, pieceIndex, begin, buf, len );
}

int
tr_ioPrefetch( tr_torrent       * tor,
               tr_piece_index_t   pieceIndex,
               uint32_t           begin,
               uint32_t           len)
{
    return readOrWritePiece( tor, TR_IO_PREFETCH, pieceIndex, begin,
                             NULL, len );
}

int
tr_ioWrite( tr_torrent       * tor,
            tr_piece_index_t   pieceIndex,
            uint32_t           begin,
            uint32_t           len,
            const uint8_t    * buf )
{
    return readOrWritePiece( tor, TR_IO_WRITE, pieceIndex, begin,
                             (uint8_t*)buf,
                             len );
}

/****
*****
****/

static tr_bool
recalculateHash( tr_torrent       * tor,
                 tr_piece_index_t   pieceIndex,
                 uint8_t          * setme )
{
    size_t   bytesLeft;
    uint32_t offset = 0;
    tr_bool  success = TRUE;
    const size_t buflen = tor->blockSize;
    void * buffer = tr_valloc( buflen );
    SHA_CTX  sha;

    assert( tor != NULL );
    assert( pieceIndex < tor->info.pieceCount );
    assert( buffer != NULL );
    assert( buflen > 0 );
    assert( setme != NULL );

    SHA1_Init( &sha );
    bytesLeft = tr_torPieceCountBytes( tor, pieceIndex );

    tr_ioPrefetch( tor, pieceIndex, offset, bytesLeft );

    while( bytesLeft )
    {
        const int len = MIN( bytesLeft, buflen );
        success = !tr_cacheReadBlock( tor->session->cache, tor, pieceIndex, offset, len, buffer );
        if( !success )
            break;
        SHA1_Update( &sha, buffer, len );
        offset += len;
        bytesLeft -= len;
    }

    if( success )
        SHA1_Final( setme, &sha );

    tr_free( buffer );
    return success;
}

tr_bool
tr_ioTestPiece( tr_torrent * tor, tr_piece_index_t piece )
{
    uint8_t hash[SHA_DIGEST_LENGTH];

    return recalculateHash( tor, piece, hash )
           && !memcmp( hash, tor->info.pieces[piece].hash, SHA_DIGEST_LENGTH );
}
