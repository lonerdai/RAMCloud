/* Copyright (c) 2014-2015 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "Crc32C.h"
#include "RamCloud.h"
#include "RpcResult.h"

namespace RAMCloud {

/**
 * Construct an RpcResult in preparation for storing it in the log.
 * This form is used when the header information is available in
 * individual pieces, while the response is stored in a Buffer
 * (typical use: during linearizable write RPCs).
 *
 * \param tableId
 *      TableId for the object modified by this RPC.
 * \param keyHash
 *      KeyHash for the object modified by this RPC.
 * \param leaseId
 *      leaseId given for this linearizable RPC.
 * \param rpcId
 *      rpcId given for this linearizable RPC.
 * \param ackId
 *      ackId given for this linearizable RPC.
 * \param respBuffer
 *      Buffer containing all chunks that will comprise this RPC's
 *      response.
 * \param respOff
 *      Byte offset in the buffer where response start
 * \param respLen
 *      Length of response.
 *      A length of 0 means that the RpcResult occupies the entire buffer
 *      starting at offset.
 */
RpcResult::RpcResult(uint64_t tableId,
                     KeyHash keyHash,
                     uint64_t leaseId,
                     uint64_t rpcId,
                     uint64_t ackId,
                     Buffer& respBuffer,
                     uint32_t respOff,
                     uint32_t respLen)
    : header(tableId,
             keyHash,
             leaseId,
             rpcId,
             ackId),
      respLength(),
      response(),
      respBuffer(&respBuffer),
      respOffset(respOff),
      indexKeyOffset(),
      indexKeyLength(),
      indexKey(NULL)
{
    // compute the actual default value
    if (respLen == 0)
        respLength = this->respBuffer->size() - respOffset;
    else
        respLength = respLen;

    //TODO(seojin): is that needed?
    void* retPtr;
    if (respBuffer.peek(respOffset, &retPtr) >= respLength)
        response = static_cast<char*>(retPtr);
}

/**
 * Construct an RpcResult in preparation for storing it in the log.
 * This form is used when the header information is available in
 * individual pieces, and memory for response is already prepared.
 * (typical use: during linearizable write RPCs).
 *
 * \param tableId
 *      TableId for the object modified by this RPC.
 * \param keyHash
 *      KeyHash for the object modified by this RPC.
 * \param leaseId
 *      leaseId given for this linearizable RPC.
 * \param rpcId
 *      rpcId given for this linearizable RPC.
 * \param ackId
 *      ackId given for this linearizable RPC.
 * \param response
 *      Buffer containing all chunks that will comprise this RPC's
 *      response.
 * \param respLen
 *      Length of response.
 */
RpcResult::RpcResult(uint64_t tableId,
                     KeyHash keyHash,
                     uint64_t leaseId,
                     uint64_t rpcId,
                     uint64_t ackId,
                     const void* response,
                     uint32_t respLen)
    : header(tableId,
             keyHash,
             leaseId,
             rpcId,
             ackId),
      respLength(respLen),
      response(response),
      respBuffer(),
      respOffset(),
      indexKeyOffset(),
      indexKeyLength(),
      indexKey(NULL)
{
}

/**
 * Construct an index RpcResult in preparation for storing it in the
 * log.
 * This form is used when the header information is available in
 * individual pieces, while the response is stored in a Buffer
 * (typical use: during linearizable write RPCs).
 *
 * \param tableId
 *      TableId for the object modified by this RPC.
 * \param indexKeyLength
 *      Length of the index key that this RPC modifies.
 * \param indexKey
 *      Pointer to the region of memory containing the index key this RPC
 *      modifies.
 * \param leaseId
 *      leaseId given for this linearizable RPC.
 * \param rpcId
 *      rpcId given for this linearizable RPC.
 * \param ackId
 *      ackId given for this linearizable RPC.
 * \param respBuffer
 *      Buffer containing all chunks that will comprise this RPC's
 *      response.
 * \param respOff
 *      Byte offset in the buffer where response start
 * \param respLen
 *      Length of response.
 *      A length of 0 means that the RpcResult occupies the entire
 *      buffer starting at offset.
 */
RpcResult::RpcResult(uint64_t tableId,
                     KeyLength indexKeyLength,
                     const void* indexKey,
                     uint64_t leaseId,
                     uint64_t rpcId,
                     uint64_t ackId,
                     Buffer& respBuffer,
                     uint32_t respOff,
                     uint32_t respLen)
    : header(tableId,
             0,
             leaseId,
             rpcId,
             ackId,
             INDEX),
      respLength(),
      response(),
      respBuffer(&respBuffer),
      respOffset(respOff),
      indexKeyOffset(),
      indexKeyLength(indexKeyLength),
      indexKey(indexKey)
{
    // compute the actual default value
    if (respLen == 0)
        respLength = this->respBuffer->size() - respOffset;
    else
        respLength = respLen;

    //TODO(seojin): is that needed?
    void* retPtr;
    if (respBuffer.peek(respOffset, &retPtr) >= respLength)
        response = static_cast<char*>(retPtr);
}

/**
 * Construct an index RpcResult in preparation for storing it in the
 * log.
 * This form is used when the header information is available in
 * individual pieces, and memory for response is already prepared.
 * (typical use: during linearizable write RPCs).
 *
 * \param tableId
 *      TableId for the object modified by this RPC.
 * \param indexKeyLength
 *      Length of the index key that this RPC modifies.
 * \param indexKey
 *      Pointer to the region of memory containing the index key this RPC
 *      modifies.
 * \param leaseId
 *      leaseId given for this linearizable RPC.
 * \param rpcId
 *      rpcId given for this linearizable RPC.
 * \param ackId
 *      ackId given for this linearizable RPC.
 * \param response
 *      Buffer containing all chunks that will comprise this RPC's
 *      response.
 * \param respLen
 *      Length of response.
 */
RpcResult::RpcResult(uint64_t tableId,
                     KeyLength indexKeyLength,
                     const void* indexKey,
                     uint64_t leaseId,
                     uint64_t rpcId,
                     uint64_t ackId,
                     const void* response,
                     uint32_t respLen)
    : header(tableId,
             0,
             leaseId,
             rpcId,
             ackId,
             INDEX),
      respLength(respLen),
      response(response),
      respBuffer(),
      respOffset(),
      indexKeyOffset(),
      indexKeyLength(indexKeyLength),
      indexKey(indexKey)
{
}

/**
 * Construct a RpcResult by deserializing an existing RpcResult. Use
 * this constructor when reading existing RpcResult from the log or from
 * individual log segments.
 *
 * \param buffer
 *      Buffer pointing to a complete serialized RpcResult. It is the
 *      caller's responsibility to make sure that the buffer passed in
 *      actually contains a full RpcResult. If it does not, then behavior
 *      is undefined.
 * \param offset
 *      Starting offset in the buffer where the RpcResult begins.
 * \param length
 *      Total length of the RpcResult in bytes.
 *      A length of 0 means that the RpcResult occupies the entire buffer
 *      starting at offset.
 */
RpcResult::RpcResult(Buffer& buffer, uint32_t offset, uint32_t length)
    : header(*buffer.getOffset<Header>(offset)),
      respLength(),
      response(),
      respBuffer(&buffer),
      respOffset(sizeof32(header) + offset),
      indexKeyOffset(sizeof32(header) + sizeof32(KeyLength) + offset),
      indexKeyLength(),
      indexKey()
{
    uint32_t headerSize = sizeof32(Header);
    if (header.type == OBJECT) {
        if (length == 0)
            respLength = downCast<uint16_t>(buffer.size() -
                                            offset - headerSize);
        else
            respLength = downCast<uint16_t>(length - headerSize);
    } else {
        indexKeyLength = *buffer.getOffset<KeyLength>(offset + headerSize);

        respOffset = headerSize + offset + sizeof32(KeyLength) + indexKeyLength;
        
        if (length == 0)
            respLength = downCast<uint16_t>(buffer.size() -
                                            offset - headerSize -
                                            sizeof32(KeyLength) -
                                            indexKeyLength);
        else
            respLength = downCast<uint16_t>(length - headerSize -
                                            sizeof32(KeyLength) -
                                            indexKeyLength);
    }
}

/**
 * Append the full RpcResult, including header, and if needed, index key, and
 * response exactly as it should be stored in the log, to a buffer.
 *
 * \param buffer
 *      The buffer to append a serialized version of this RpcResult to.
 */
void
RpcResult::assembleForLog(Buffer& buffer)
{
    header.checksum = computeChecksum();
    buffer.appendExternal(&header, sizeof32(header));
    
    // Append index key length and index key if needed.
    if (header.type == INDEX) {
        appendIndexKeyToBuffer(buffer);
    }
    
    //buffer.appendExternal(&clusterTime, 8);
    appendRespToBuffer(buffer);
}

/**
 * Copy the full serialized RpcResult including the header, and if needed, index
 * key, and the response to a contiguous region of memory memBlock.
 *
 * \param memBlock
 *      Destination to copy a serialized version of this RpcResult to.
 *      The caller must ensure that sufficient memory is allocated for the
 *      complete RpcResult to be stored.
 */
void
RpcResult::assembleForLog(void* memBlock)
{
    uint8_t *dst = reinterpret_cast<uint8_t*>(memBlock);
    header.checksum = computeChecksum();

    memcpy(dst, &header, sizeof32(header));
    //memcpy(dst + sizeof32(header), &clusterTime, 8);
    if (header.type == OBJECT) {
        memcpy(dst + sizeof32(header), getResp(), respLength);
    } else {
        // Index RPC
        KeyLength keyLen = getIndexKeyLength();
        memcpy(dst + sizeof32(header), &keyLen, sizeof32(KeyLength));
        memcpy(dst + sizeof32(header) + sizeof32(KeyLength),
               getIndexKey(), keyLen);
        memcpy(dst + sizeof32(header) + sizeof32(KeyLength) +
               keyLen, getResp(), respLength);
    }
}

/**
 * Append the response portion of this RpcResult to a provided
 * buffer.
 *
 * \param buffer
 *      The buffer to append the response to.
 */
void
RpcResult::appendRespToBuffer(Buffer& buffer)
{
    if (response) {
        buffer.appendExternal(response, getRespLength());
        return;
    }

    Buffer::Iterator it(respBuffer, respOffset, getRespLength());

    while (!it.isDone()) {
        buffer.appendExternal(it.getData(), it.getLength());
        it.next();
    }
}

/**
 * Append the index key modified by this RPC to a provided buffer.
 *
 * \param buffer
 *      The buffer to append the index key to.
 */
void
RpcResult::appendIndexKeyToBuffer(Buffer& buffer)
{
    KeyLength keyLen = getIndexKeyLength();
    buffer.appendCopy<KeyLength>(&keyLen);
    
    if (indexKey) {
        buffer.append(indexKey, keyLen);
        return;
    }

    buffer.append(respBuffer, indexKeyOffset, keyLen);
}

/**
 * Obtain the type of RPC this record refers to - either OBJECT or INDEX.
 */
RpcResult::RecordType
RpcResult::getType()
{
    return header.type;
}

/**
 * Obtain the 64-bit table identifier for the object modified by this RPC.
 */
uint64_t
RpcResult::getTableId()
{
    return header.tableId;
}

/**
 * Obtain the KeyHash for the object modified by this RPC.
 */
KeyHash
RpcResult::getKeyHash()
{
    return header.keyHash;
}

/**
 * Obtain the 64-bit lease identifier given for this linearizable RPC.
 */
uint64_t
RpcResult::getLeaseId()
{
    return header.leaseId;
}

/**
 * Obtain the 64-bit RPC identifier given for this linearizable RPC.
 */
uint64_t
RpcResult::getRpcId()
{
    return header.rpcId;
}

/**
 * Obtain the 64-bit acknowledgment value given for this linearizable RPC.
 */
uint64_t
RpcResult::getAckId()
{
    return header.ackId;
}

/**
 * Obtain a pointer to a contiguous copy of this RPC's response.
 *
 * \param[out] length
 *      The length of the RPC's response in bytes.
 *
 * \return
 *      NULL if the RpcResult is malformed,
 *      a pointer to a contiguous copy of the RPC's response otherwise
 */
const void*
RpcResult::getResp(uint32_t *length)
{
    if (length)
        *length = respLength;

    if (response)
        return static_cast<const uint8_t*>(response);
    else
        return respBuffer->getRange(respOffset, respLength);
}

uint32_t
RpcResult::getRespLength()
{
    return respLength;
}

/**
 * Obtain a pointer to the index key modified by this RPC.
 *
 * \param[out] length
 *      The length of the index key in bytes.
 *
 * \return
 *      NULL if the RpcResult is malformed,
 *      a pointer to a contiguous copy of the index key otherwise.
 */
const void*
RpcResult::getIndexKey(KeyLength *length)
{
    if (length)
        *length = indexKeyLength;

    if (indexKey)
        return indexKey;
    else
        return respBuffer->getRange(indexKeyOffset, getIndexKeyLength());
}

/**
 * Obtain the length of the index key this RPC modifies.
 */
KeyLength
RpcResult::getIndexKeyLength()
{
    return indexKeyLength;
}

/**
 * Compute a checksum on the RpcResult and determine whether or not it matches
 * what is stored in the RpcResult. Returns true if the checksum looks ok,
 * otherwise returns false.
 */
bool
RpcResult::checkIntegrity()
{
    return computeChecksum() == header.checksum;
}

/**
 * Obtain the total size of the RpcResult including its header.
 */
uint32_t
RpcResult::getSerializedLength()
{
    if (header.type == OBJECT) {
        return sizeof32(header) + respLength;
    } else { // INDEX
        return sizeof32(header) + sizeof32(KeyLength) + getIndexKeyLength() +
                respLength;
    }
}

/**
 * Compute the RpcResult's checksum and return it.
 */
uint32_t
RpcResult::computeChecksum()
{
    assert(OFFSET_OF(Header, checksum) ==
        (sizeof(header) - sizeof(header.checksum)));

    Crc32C crc;
    crc.update(&header, downCast<uint32_t>(OFFSET_OF(Header, checksum)));

    if (header.type == INDEX) {
        if (indexKey) {
            crc.update(indexKey, getIndexKeyLength());
        } else {
            crc.update(*respBuffer, indexKeyOffset, getIndexKeyLength());
        }
    }

    if (response) {
        crc.update(response, getRespLength());
    } else {
        crc.update(*respBuffer, respOffset, getRespLength());
    }

    return crc.getResult();
}

} // namespace RAMCloud
