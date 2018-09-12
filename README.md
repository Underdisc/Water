# Water

This contains an implementation of a Gerstner and FFT water simulation.

## Compiling

This currently only compiles under cl and g++. I have not successfully linked using g++ due to some problems with compiling opengl programs under cygwin.

### Visual Studio

To compile with visual studio, open the solution located in `build/vs/`. Once the solution is open in visual studio, the project setting for the working directory will need to changed. Open up the project properties and under `Configuration Properties -> Debugging`, there should be a `Working Directory` field. Change this to `$(ProjectDir)../working` and you'll be able to run the program directly from visual studio. Alternatively, moving the executable into `build/working/` after compiling will also work.

Once the working directory is updated, compile for release x86 and everything should be good to go.

### G++

As I said, I have not gotten this to work with g++ yet, however, I am only dealing with a linker issue that is specifically related to opengl. Due to better support, g++ on linux might work fine. There is a makefile in `build/make/` to compile and link. Just run make directly from that directory.
