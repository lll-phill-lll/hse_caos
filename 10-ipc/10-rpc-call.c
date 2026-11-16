

RpcClient *client = rpc_connect("127.0.0.1:5000");
ResultRequest request = {.block_id = 3};
ResultReply reply = client.GetResult(request);




