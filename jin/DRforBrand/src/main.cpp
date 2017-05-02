#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( int argc, const char** argv ){
    ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context
    
    //ofxArgParser::init(argc, argv); // argument parsing
    
    vector<string> myArgs;
    
    for (int i = 0; i < argc - 1; i++) {
        if (ofToString(argv[i]) == "--arguments") {
            for (i++; i < argc; i++) {
                myArgs.push_back(ofToString(argv[i]));
            }
        }
    }
    
    ofApp *app = new ofApp();
    app->arguments = myArgs;
    
    
    //ofApp *app = new ofApp();
    
    //app->arguments = vector<string>(argv, argv + argc);
    
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(app);
    
    
}
