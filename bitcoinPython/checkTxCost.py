from bitcoin.rpc import RawProxy

p = RawProxy()

txInput = raw_input("Enter transaction id ")

rawTx = p.getrawtransaction(txInput)

decodedTx = p.decoderawtransaction(rawTx)

sumIn = 0
sumOut = 0
for input in decodedTx['vin']:
        vinTxId = input['txid']
        rawVinTx = p.getrawtransaction(vinTxId)
        decodedVinTx = p.decoderawtransaction(rawVinTx)
        vinOut = input['vout']
        for output in decodedVinTx['vout']:
                if(vinOut == output['n']):
                        sumIn += output['value']

for output in decodedTx['vout']:
        sumOut += output['value']

print(sumIn - sumOut)