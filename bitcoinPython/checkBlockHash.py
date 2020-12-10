from bitcoin.rpc import RawProxy
from hashlib import sha256
import binascii
import struct

def hexify(value, type):
    return binascii.hexlify(struct.Struct(type).pack(value))

p = RawProxy()

blockheight = 1337

blockhash = p.getblockhash(blockheight)
block = p.getblock(blockhash)

block_version = block['version']
previous_hash = block['previousblockhash']
hash_merkle_root = block['merkleroot']
bits = block['bits']
timestamp = block['time']
nonce = block['nonce']

header_hex = '{block_version}{previous_hash}{hash_merkle_root}{timestamp}{bits}{nonce}'.format(
    block_version = hexify(block_version, '<L'),
    previous_hash = binascii.hexlify(previous_hash.decode('hex')[::-1]),
    hash_merkle_root = binascii.hexlify(hash_merkle_root.decode('hex')[::-1]),
    timestamp = hexify(timestamp, '<L'),
    bits = binascii.hexlify(bits.decode('hex')[::-1]),
    nonce = hexify(nonce, '<L')
)

block = header_hex.decode('hex')
hash = sha256(sha256(block).digest()).digest()
block_hash = hash[::-1].encode('hex_codec')

blockhash = p.getblockhash(blockheight + 1)
block = p.getblock(blockhash)

if (block['previousblockhash'] == block_hash):
    print("Hashes do match. \nHash: " + block_hash)
else :
    print("Hashes do not match.")