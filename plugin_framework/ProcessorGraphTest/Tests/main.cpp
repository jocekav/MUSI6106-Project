//
// Created by Vedant Kalbag on 04/04/22.
//

//CATCH_CONFIG_RUNNER tells the catch library that this
//project will now explicitly call for the tests to be run.
#define CATCH_CONFIG_RUNNER
#include "../3rd-party/catch2/catch.hpp"
#define TEST true

/*
* runCatchTests will cause Catch to go ahead and
* run your tests (that are contained in the tests.cpp file.
* to do that, it needs access to the command line
* args - argc and argv. It returns an integer that
* ultimately gets passed back up to the operating system.
* See the if statement at the top of main for
* a better overview.
*/
int runCatchTests(int argc, char* const argv[])
{
    //This line of code causes the Catch library to
    //run the tests in the project.
    return Catch::Session().run(argc, argv);
}
#include "JuceHeader.h"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char* argv[])
{
    juce::initialiseJuce_GUI();
    int result = Catch::Session().run(argc, argv);
    juce::shutdownJuce_GUI();
    return result;
}
