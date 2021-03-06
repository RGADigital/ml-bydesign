#include "ofApp.h"
#include <stdlib.h>
#include <algorithm>

const string allowed_ext[] = {"jpg", "png", "gif", "jpeg"};

vector<ofFile> ofApp::scan_dir_imgs(ofDirectory dir){
    ofDirectory new_dir;
    
    vector<ofFile> imageFiles;
    
    int size = dir.listDir();
    for (int i = 0; i < size; i++){
        if (dir.getFile(i).isDirectory()){
            new_dir = ofDirectory(dir.getFile(i).getAbsolutePath());
            new_dir.listDir();
            new_dir.sort();
            scan_dir_imgs(new_dir);
        } else if (std::find(std::begin(allowed_ext),
                             std::end(allowed_ext),
                             dir.getFile(i).getExtension()) != std::end(allowed_ext)) {
            imageFiles.push_back(dir.getFile(i));
            
            if ( dir.getFile(i).getExtension() != "jpg" ) cout << dir.getFile(i).getExtension() << endl;
        }
    }
    
    return imageFiles;
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    // SETUP
    // imageDir, imageSavePath = location of images, path to save the final grid image
    // nx, ny = size of the grid (make sure there are at least nx*ny images in imageDir!)
    // w, h = downsample (or scale up) for source images prior to encoding!
    // displayW, displayH = resolution of the individual thumbnails for your output image - be careful about going over your maximum texture size on graphics card - 5000x5000 may work, but 10000x10000 may not
    // perplexity, theta (for t-SNE, see 'example' for explanation of these)
    
    
    cout << "args: " << arguments.at(0) << ", 2: " << arguments.at(1) << endl;
    
    string brand1, brand2;
    int dimension;
    
    if (arguments.size() > 0 ) {
        brand1 = arguments.at(0);
        brand2 = arguments.at(1);
        dimension = std::stoi(arguments.at(2));
    } else {
        brand1 = "nike";
        brand2 = "adidas";
        dimension = 15;
    }

    
    string imageDir = "instagram-scraper/"  + brand1;
    string imageDir2 = "instagram-scraper/" + brand2;
    string imageSavePath = "tsne_grid_" + brand1 + "_" + brand2 + "_" + arguments.at(2) + ".png";
    string tintImageSavePath = "tsne_grid_tint_" + brand1 + "_" + brand2 + "_" + arguments.at(2) + ".png";
    string categoryImageSavePath = "tsne_categories_" + brand1 + "_" + brand2 + "_" + arguments.at(2) + ".png";
    string wordsSavePath = "tsne_categories_" + brand1 + "_" + brand2 + "_" + arguments.at(2) + ".txt";
    
    nx = dimension;
    ny = dimension;
    w = 256; //do not go lower than 256 - it will work, but results won't be as good
    h = 256;
    displayW = 128;
    displayH = 128;
    perplexity = 50; // corresponds to "number of neighbors", somewhere in the range 10-100 is good
    theta = 0.5; // lower is more "accurate" but takes longer, don't need to change this
    
    int numToShowFromFirst = nx*ny/2;
    
    // to run the instagram downloads directly from openframeworks... needs a little work still to get the photos saved to the correct directory (outside the app execution space)
//    std::string instagramCmdStr = "python " + ofToDataPath("instagram-scraper/instagram_scraper/app.py", true) + " " + brand1 + "," + brand2;
//    const char *instagramCmd = instagramCmdStr.c_str();
    
//    imageDir = ofToDataPath(imageDir);
//    imageDir2 = ofToDataPath(imageDir2);
//    
//    std::system(instagramCmd);
    
    
    /////////////////////////////////////////////////////////////////////
    // CCV activations -> t-SNE embedding -> grid assignments
    
    // get images recursively from directory
    ofLog() << "Gathering images...";
    ofDirectory dir (imageDir);
    ofDirectory dir2 (imageDir2);
    imageFiles1 = scan_dir_imgs(dir);
    numOfFilesOnFirst = imageFiles1.size();
    imageFiles2 = scan_dir_imgs(dir2);
    
    int numImagesToUse = nx*ny;
    numToShowFromFirst = numImagesToUse/2;
    numToShowFromSecond = numImagesToUse/2;
    //nx = ny = floor(sqrt(imageFiles.size()));
    
//    if ((imageFiles1.size() + imageFiles2.size())  < nx * ny) {
//        ofLog(OF_LOG_ERROR, "There are fewer images in the directory than the grid size requested (nx*ny="+ofToString((nx*ny))+"). Exiting to save you trouble...");
//        ofExit(); // not enough images to fill the grid, so quitting
//    }
//    
//    if(numOfFilesOnFirst < numToShowFromFirst) {
//        numToShowFromFirst = numOfFilesOnFirst;
//    }
//    
//    // if the 2nd account doesnt have enough images to fill 1/2 the image
//    if(imageFiles.size() - numToShowFromFirst < numToShowFromFirst ) {
//        numToShowFromFirst += imageFiles.size()-numToShowFromFirst*2;
//    }

    
    if( imageFiles1.size() < numToShowFromFirst) {
        numToShowFromFirst = imageFiles1.size();
        numToShowFromSecond = numImagesToUse - numToShowFromFirst;
    }
    
    if ( imageFiles2.size() < numToShowFromSecond ) {
        numToShowFromSecond = imageFiles2.size();
        numToShowFromFirst = numImagesToUse - numToShowFromSecond;
    }
    
    if ((numToShowFromFirst + numToShowFromSecond)  < numImagesToUse) {
            ofLog(OF_LOG_ERROR, "There are fewer images in the directory than the grid size requested (nx*ny="+ofToString((nx*ny))+"). Exiting to save you trouble...");
            ofExit(); // not enough images to fill the grid, so quitting
    }
    
    // setup ofxCcv
    ccv.setup("image-net-2012.sqlite3");
    
    // load first account the images
    for(int i=0; i<numToShowFromFirst; i++) {
        if (i % 20 == 0)    ofLog() << " - loading image "<<i<<" / "<<numToShowFromFirst<<" ("<<imageFiles1.size()<<" in dir)";
        images.push_back(ofImage());
        images.back().load(imageFiles1[i]);
        
    }
    
    // load the images from the next account
    for(int i=0; i<numToShowFromSecond; i++) {
        if (i % 20 == 0) {
            
            ofLog() << " - loading image "<<i<<" / "<<numToShowFromSecond<<" ("<<imageFiles2.size()<<" in dir)";
        }
        images.push_back(ofImage());
        images.back().load(imageFiles2[i]);
        
    }
    
    
    
    // resize images to w x h
    for (int i=0; i<images.size(); i++) {
        if (images[i].getWidth() > images[i].getHeight()) {
            images[i].crop((images[i].getWidth()-images[i].getHeight()) * 0.5, 0, images[i].getHeight(), images[i].getHeight());
        }
        else if (images[i].getHeight() > images[i].getWidth()) {
            images[i].crop(0, (images[i].getHeight()-images[i].getWidth()) * 0.5, images[i].getWidth(), images[i].getWidth());
        }
        images[i].resize(w, h);
        //images[i].setImageType(OF_IMAGE_GRAYSCALE);
    }
    
    
    myfile.open (ofToDataPath(wordsSavePath),ofFile::WriteOnly);
    
    // encode all of the images with ofxCcv
    ofLog() << "Encoding images...";
    for (int i=0; i<images.size(); i++) {
        if (i % 20 == 0) ofLog() << " - encoding image "<<i<<" / "<<images.size();
        vector<float> encoding = ccv.encode(images[i], ccv.numLayers()-1);
        encodings.push_back(encoding);
        
        // classify
        
        myfile << "\nIMAGE " << i << "\n";
        
        classifiers = ccv.classify(images[i]);
        
        
        for(ofxCcv::Classification a: classifiers){
            myfile << a.imageNetName << " " << a.confidence  << "\n"; // << " : " << a.confidence
            
            // add to the classifiers vector
            if( i < numToShowFromFirst ) {
                classifiers1.push_back(a);
            } else {
                classifiers2.push_back(a);
            }
        }
    }
    myfile.flush();
    myfile.close();
    
//    for (std::vector<ofxCcv::Classification>::const_iterator i = classifiers.begin(); i != classifiers.end(); ++i)
//        std::cout << *i << '    ';
    
    
    // run t-SNE and load image points to imagePoints
    tsneVecs = tsne.run(encodings, 2, perplexity, theta, true);
    ofLog() << "Run t-SNE on images ";
    
    // solve assignment grid
    vector<ofVec2f> tsnePoints; // convert vector<double> to ofVec2f
    for (auto t : tsneVecs) tsnePoints.push_back(ofVec2f(t[0], t[1]));
    vector<ofVec2f> gridPoints = makeGrid(nx, ny);
    solvedGrid = solver.match(tsnePoints, gridPoints, false);
    
    // save
    ofFbo fbo;
    fbo.allocate(nx * displayW, ny * displayH);
    fbo.begin();
    ofClear(0, 0);
    ofBackground(0);
    for (int i=0; i<solvedGrid.size(); i++) {
        float x = (fbo.getWidth() - displayW) * solvedGrid[i].x;
        float y = (fbo.getHeight() - displayH) * solvedGrid[i].y;
        ofSetColor(255, 255, 255, 255);
        images[i].draw(x, y, displayW, displayH);
    }
    fbo.end();
    ofImage img;
    fbo.readToPixels(img);
    img.save(imageSavePath);
    
    // save
    ofFbo fbo2;
    fbo2.allocate(nx * displayW, ny * displayH);
    fbo2.begin();
    ofClear(0, 0);
    ofBackground(0);
    for (int i=0; i<solvedGrid.size(); i++) {
        float x = (fbo.getWidth() - displayW) * solvedGrid[i].x;
        float y = (fbo.getHeight() - displayH) * solvedGrid[i].y;
        ofSetColor(255, 255, 255, 255);
        ofImage greyImage = images[i];
        greyImage.setImageType(OF_IMAGE_GRAYSCALE);
        greyImage.draw(x, y, displayW, displayH);
        //if (isTint) {
        if (numToShowFromFirst <= i ) {
            ofSetColor(255, 0, 0, 100); // set color to red
        } else {
            ofSetColor(0, 0, 255, 100); // set color to blue
        }
        ofRect(x, y, displayW, displayH); // draw rectangle (red)
        //}
    }
    fbo2.end();
    ofImage img2;
    fbo2.readToPixels(img2);
    img2.save(tintImageSavePath);
    
    // categories image overlay
    ofFbo fbo3;
    fbo3.allocate(nx * displayW, ny * displayH);
    fbo3.begin();
    ofClear(0, 0);
    ofEnableAlphaBlending();
    ofBackground(0,0,0,0);
    for (int i=0; i<solvedGrid.size(); i++) {
        float x = (fbo.getWidth() - displayW) * solvedGrid[i].x + 10;
        float y = (fbo.getHeight() - displayH) * solvedGrid[i].y;
        ofSetColor(255, 255, 255, 255);
        
//        ofImage greyImage = images[i];
        
        int displace = 0;
        classifiers = ccv.classify(images[i]);
        
        for(ofxCcv::Classification a: classifiers){
            ofDrawBitmapString(a.imageNetName.substr(0, a.imageNetName.find(",", 0)), x, y+displace);
       //     myfile << a.imageNetName  << "\n"; // << " : " << a.confidence
//            cout << a.imageNetName  << std::endl;
            
            displace += 14;
        }
        
    }
    fbo3.end();
    ofImage img3;
    fbo3.readToPixels(img3);
    img3.save(categoryImageSavePath);
    
    
    // setup gui
    gui.setup();
    gui.add(scale.set("scale", 1.0, 0.0, 1.0));
    //    isTint = new ofxToggle("Tint", true);
    gui.add(isTint.set(false));
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    ofPushMatrix();
    ofTranslate(-ofGetMouseX() * (scale - 0.5), -ofGetMouseY() * (scale - 0.5));
    for (int i=0; i<solvedGrid.size(); i++) {
        float x = scale * (nx - 1) * w * solvedGrid[i].x;
        float y = scale * (ny - 1) * h * solvedGrid[i].y;
        ofSetColor(255, 255, 255, 255);
        images[i].draw(x, y, scale * images[i].getWidth(), scale * images[i].getHeight());
        if (isTint) {
            if (numOfFilesOnFirst <= i ) {
                ofSetColor(255, 0, 0, 125); // set color to red
            } else {
                ofSetColor(0, 0, 255, 125); // set color to blue
            }
            ofRect(x, y, scale * images[i].getWidth(), scale * images[i].getHeight()); // draw rectangle (red)
        }
    }
    ofPopMatrix();
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
