# SearchEngineFinalProject with Ian
Web Crawler
1. socket: master slave
2. qt: UI
3. crawler: multiprocessing mechanism

Server Usage:
sockServer sS = new sockServer();
sS.run(); // start server
sS.check(); // check where goes wrong
sS.renewfd(); // reset

Client Usage:
sockClient sC = new sockClient();
sC.run(); // start to connect to server
sC.check(); // check where goes wrong
