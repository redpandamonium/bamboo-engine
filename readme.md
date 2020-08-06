## Setup

You need the bincrafters conan repository. You can add it with:
```
conan remote add bincrafters "https://api.bintray.com/conan/bincrafters/public-conan"
```

## Style

Error handling boils down to two cases: Fatal errors and non-fatal errors. Fatal errors should
throw an exception which will get handled and the program terminated. Non-fatal errors need to be handled by the 
caller of a function. Use ``bbge::result<Ok, Err>`` for this.