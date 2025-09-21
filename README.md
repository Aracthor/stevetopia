# Stevetopia

An ongoing game project with still unclear goal.
The only constant being : there are Steves, lot of it.

![Stevetopia -- demo](/screenshots/demo.png)

This project is mostly a proof-of-concept of [Hatcher engine](https://github.com/Aracthor/hatcher).

## Running

Compile and run the native version by running:
```
make native_release
./bin/native_release
```

Compile and run the webasm version by running :
```
make webasm_release
node hatcher/LocalServer.js
```
And then going to http://127.0.0.1:4242/index.html with a web browser.
