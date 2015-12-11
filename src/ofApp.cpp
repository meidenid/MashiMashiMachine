#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    camWidth = 640;
    camHeight = 480;
    ofBackground(200, 200, 200);
    ofSetFrameRate(60);
    //sato.loadImage("an.jpg");
    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(camWidth, camHeight);
    ofSetVerticalSync(true);
    capCount        = 0;
    capNum          = 0;
    eraseId         = 0;
    drawId          = 0;
    copyCapNum      = 0;
    copyCapCount    = 0;
    backPastNum     = 0;
    backPastCount   = 0;
    pastNum         = 0;
    pastCount       = 0;
    regularNum      = 0;
    clickFlg        = true;
    threshold       = 30;
    speedFlg        = 0;
    flowAve         = 0;
    speedScale      = 1;
    flowWaitFlame   = 500;
    for(int i=0; i<COLNUM; i++){
        eraseColor[i] = ofColor(0,100+i*5,0);
        drawColor[i] = ofColor(0,0,0);
    }
    for(int i=0; i<100; i++){
        particle[i].x = camWidth/2;
        particle[i].y = camHeight/2;
        particle[i].z = ofRandom(1,10);
        
        particleVec[i].x = ofRandom(-20, 20);
        particleVec[i].y = ofRandom(-20, 20);
        
        particleCol[i].set(ofRandom(250), ofRandom(250), ofRandom(250));
        cout << particleVec[0].x << endl;
    }

    capNum = 0;
    // cap.allocate(camWidth, camHeight, OF_IMAGE_COLOR_ALPHA);
    
    //CV
    contourFinder.setMinAreaRadius(10);
    contourFinder.setMaxAreaRadius(200);
    background.setLearningTime(900);
    background.setThresholdValue(20);
    
    //GUI
    ofxGuiSetTextPadding(4);
    ofxGuiSetDefaultWidth(300);
    ofxGuiSetDefaultHeight(18);
    gui.setup();
    gui.add(capSld.setup("capmax", 10, 1, CAPMAX));
    gui.add(saveSld.setup("savemax", 5, 1, SAVEMAX));
    gui.add(threSld.setup("threshold",30,5,255));
    gui.add(speed.setup("speeed",1,0.5,3));
    gui.add(regularSld.setup("regularNum",1,0,10));
    gui.add(random.setup("random"));
    gui.add(captur.setup("captur"));
    gui.add(colSld.setup("color",ofColor(255,255,255),ofColor(0,0),ofColor(255,255)));
    reset.addListener(this, &ofApp::presResetbutton);
    gui.add(changeCol.setup("changeColor",0,0,3));
    gui.add(reset.setup("reset"));
    gui.add(reguner.setup("reguner",false));
    gui.add(mode.setup("mode",true));
    gui.add(drawpast.setup("past",false));
    gui.add(drawpastBack.setup("pastBack",false));
    
    //オプティカルフロぅ
    cv.setup();
    cv.add(pyrScale.setup("pyrScale",.5,0,1));
    cv.add(level.setup("level",4,1,8));
    cv.add(winsize.setup("level",8,4,64));
    cv.add(iteration.setup("interation",2,2,8));
    cv.add(polyN.setup("polyN",7,5,10));
    cv.add(polySigma.setup("polySigma",1.5,1.1,2));
    cv.add(OPTFLOW.setup("OPTFLOW",false));
    cv.add(useFarneback.setup("useFarneback",false));
    cv.add(winSize.setup("winSize",32,4,64));
    cv.add(maxLevel.setup("maxLevel",3,0,8));
    cv.add(level.setup("maxfeatures",200,2,1000));
    cv.add(qualityLevel.setup("levels",0.01,0.01,.02));
    cv.add(minDistance.setup("minDistane",4,1,16));
    
    curFlow = &farneback;

    
    //http://www.slideshare.net/tado/media-art-ii-2013-6openframeworks-addon-2-ofxopencv-ofxcv
    
    isCapFlowFilled = true;
    
}

//--------------------------------------------------------------
void ofApp::update() {
    vidGrabber.update();
    threshold = threSld;
    
    //3秒ごとにregularNumの一部にcapを代入
    if(ofGetSeconds() % 3 == 0 && regularFlg){
        if(capNum == 0){
            for(int i=0; i<capmax; i++){
                capRegular[regularNum][i] = cap[savemax-1][i];
            }
        }else{
            for(int i=0; i<capmax; i++){
                capRegular[regularNum][i] = cap[capNum-1][i];
            }
        }
        regularNum++;
        //cout << regularNum << endl;
        if(regularNum == REGMAX-1){regularNum = 0;}
        
        regularFlg = false;
        
        for(int i=0; i<regularSld; i++){
            regularRandNum[i] = ofRandom(REGMAX);
        }
    }
    if(ofGetSeconds() %3 == 1){regularFlg = true;}
    
    
    //capturボタンが押されたらcapDamに現在の動画を代入
    //drawPast,drawPastBackに使われる奴
    if(captur){
        for(int i=0; i< savemax; i++){
            for(int j=0; j<capmax; j++){
                capDam[i][j] = cap[i][j];
            }
        }
        copyCapCount    = capCount;
        pastCount       = capCount;
        backPastCount   = capCount;
    
        copyCapNum      = capNum;
        pastNum         = capNum;
        backPastNum     = capNum;
        }
    
    //おぷてぃかるふろぅ
    if(vidGrabber.isFrameNew()){
        if(useFarneback){
            curFlow = &farneback;
            farneback.setPyramidScale(pyrScale);
            farneback.setPolySigma(polySigma);
            farneback.setNumLevels(level);
            farneback.setNumIterations(iteration);
            farneback.setPolyN(polyN);
            farneback.setUseGaussian(OPTFLOW);
            farneback.setWindowSize(winsize);
        }else{
            curFlow = &pyrLk;
            pyrLk.setMaxFeatures(maxFeatures);
            pyrLk.setQualityLevel(qualityLevel);
            pyrLk.setWindowSize(winSize);
            pyrLk.setMaxLevel(maxLevel);
            pyrLk.setMinDistance(minDistance);
        }
    }
    curFlow->calcOpticalFlow(vidGrabber);
}

//--------------------------------------------------------------
void ofApp::draw() {

    int sec = ofGetSeconds();

    vector<ofVec2f> pyrMotion = pyrLk.getMotion();
    
    ofVec2f pyrAve;
    for(int i=0; i<pyrMotion.size(); i++){
        pyrAve.x += pyrMotion[i].x;
        pyrAve.y += pyrMotion[i].y;
    }
    pyrAve.x /= pyrMotion.size() ;
    pyrAve.y /= pyrMotion.size() ;
    flowAve = abs(pyrAve.x) + abs(pyrAve.y);
    
    ofEnableAlphaBlending();
    //最初初期化してないの描画してるかも
    //img.draw(0,0);
    
    //動体の情報
    unsigned char* cp = vidGrabber.getPixels();
    img.setFromPixels(cp,camWidth,camHeight,OF_IMAGE_COLOR);
    
    //透過した奴をcap配列(分身)に代入
    //cout << capNum << endl;
    cap[capNum][capCount].setFromPixels(onAlpha(cp), camWidth, camHeight, OF_IMAGE_COLOR_ALPHA);
//    capFlow[capNum*capmax + capCount] = cap[capNum][capCount];

    if (isCapFlowFilled) {
        for (int i; i < SAVEMAX * CAPMAX; i++) {
            capFlow[i] = cap[capNum][capCount];
            flowValue[i] = abs(pyrAve.x)+abs(pyrAve.y);
            isCapFlowFilled = false;
        }
    }
    
    for(int i=0; i < savemax * capmax; i++){
        if (i == savemax * capmax-1) {
            capFlow[i] = cap[capNum][capCount];
            flowValue[i] = abs(pyrAve.x)+abs(pyrAve.y);
        } else {
            capFlow[i] = capFlow[i+1];
            flowValue[i] = flowValue[i+1];
        }
    }

    drawEffect();
    if(mode){drawCap();}
    if(!mode){drawLocus();}
    drawCol();
    drawSub();
    if(drawpast){drawPast();}
    if(drawpastBack){drawPastBack();}
    if(reguner){drawRegular();}
    drawRandom();
    
    drawFlowCap();
    ofSetColor(colSld);
    cap[capNum][capCount].draw(0,0);
    
    capCount++;
    
    //速度変化 1.5倍速の挙動がおかしい。
    if(speedScale == 2){
        capCount++;
        pastCount++;
        backPastCount--;
    }else if(speedScale == 3){
        capCount+=2;
        pastCount+=2;
        backPastCount-=2;
    }else if(speedScale == 0.5){
        if(speedFlg){
            capCount--;
            pastCount--;
            backPastCount++;
        }
        speedFlg = !speedFlg;
    }
    
    //capmaxフレームごとに次のバッファへ。
    if(capCount >= capmax) {
        if(capNum == savemax-1){
            capNum = 0;
        }else{
            capNum++;
        }
        capCount = 0;
    }
    
    //ofDisableAlphaBlending();
    
    curFlow -> draw(0,0,camWidth,camHeight);
    gui.setPosition(700, 10);
    cv.setPosition(1050, 10);
    gui.draw();
    cv.draw();
    
    float fps = ofGetFrameRate();
    ofDrawBitmapStringHighlight("sec: " + ofToString(sec), 20,20);
    ofDrawBitmapStringHighlight("pyrAve: "+ ofToString(pyrAve), 20, 40);
    ofDrawBitmapStringHighlight("flowAve: "+ ofToString(flowAve), 20, 60);
    ofDrawBitmapStringHighlight("flowValue: "+ ofToString(flowValue[capNum*capmax + capCount]), 20, 80);
    ofDrawBitmapStringHighlight("speed: "+ ofToString(speedScale),20,100);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

    if(key == 's'){
        speedScale = speed;
        if(speedScale < 0.75){
            speedScale=0.5;
        }else if (speedScale < 1.5){
            speedScale = 1;
        }else if(speedScale < 2.5){
            speedScale = 2;
        }else if(speedScale < 3){
            speedScale = 3;
        }
    }
    
    if(key == 'c'){
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    
    if(clickFlg){
        if(eraseId == COLNUM){eraseId = 0;}
        eraseColor[eraseId] = img.getColor(x,y);
        eraseId++;
    }else{
        if(drawId == COLNUM){drawId = 0;}
        drawColor[drawId] = img.getColor(x,y);
        drawId++;
    }
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
    
}

unsigned char* ofApp::onAlpha(unsigned char *cp){
    
    //mpに現在フレームを透過して代入
    unsigned char* mp;
    mp = (unsigned char*)malloc(camWidth * camHeight * 4);
    if (img.bAllocated()) {
        for (int y = 0; y < camHeight-1; y++) {
            for (int x = 0; x < camWidth-1; x++) {
                ofColor c = img.getColor(x, y);
                ofSetColor(c.r, c.g, c.b);
                switch (changeCol) {
                    case 0:
                        mp[(camWidth * y + x) * 4 + 2] = c.b;
                        mp[(camWidth * y + x) * 4 + 1] = c.g;
                        mp[(camWidth * y + x) * 4]     = c.r;
                        break;
                        
                    case 1:
                        mp[(camWidth * y + x) * 4 + 2] = 255-c.b;
                        mp[(camWidth * y + x) * 4 + 1] = 255-c.g;
                        mp[(camWidth * y + x) * 4]     = 255-c.r;
                        break;
                        
                    case 2:
                        mp[(camWidth * y + x) * 4 + 2] = c.r;
                        mp[(camWidth * y + x) * 4 + 1] = c.b;
                        mp[(camWidth * y + x) * 4]     = c.g;
                        break;
                        
                    case 3:
                        mp[(camWidth * y + x) * 4 + 2] = c.g;
                        mp[(camWidth * y + x) * 4 + 1] = c.r;
                        mp[(camWidth * y + x) * 4]     = c.b;
                        break;
                        
                    default:
                        break;
                }
                mp[(camWidth * y + x) * 4 + 3] = 255;
                //指定色を透過
                for(int i = 0; i<COLNUM; i++){
                    if(((c.g > c.r && c.g > c.b ) && (c.g-c.r > threshold)) ||
                       ((abs(c.r-eraseColor[i].r) < threshold) && (abs(c.g-eraseColor[i].g) < threshold) && (abs(c.b-eraseColor[i].b) < threshold))){
                        mp[(camWidth * y + x) * 4 + 3] = 0;
                        break;
                    }
                }
                
                //指定色を描画
                for(int i = 0; i < COLNUM; i++){
                    if((abs(c.r - drawColor[i].r)) < 5 && (abs(c.r - drawColor[i].r)) && (abs(c.r - drawColor[i].r))){
                        mp[(camWidth * y + x) * 4 + 3] = 255;
                    }
                }
            }
        }
    }
    free(mp);
    return mp;
}

void ofApp::drawCol(){
    for(int i =0; i<COLNUM/5; i++){
        for(int j = 0; j<5; j++){
            ofSetColor(eraseColor[i*5+j]);
            ofRect(camWidth+80*j,camHeight+0+i*50,50,50);
            ofSetColor(drawColor[i*5+j]);
            ofRect(camWidth+80*j,camHeight+100+i*50,50,50);
        }
    }
}

void ofApp::drawCap(){
    int num = capNum;
    ofColor col = colSld;
    
    for(int i=0; i<savemax; i++){
        num--;
        if(num < 0){num = savemax-1;}
        if(cap[num][capmax-1].bAllocated()){
            col.a = (savemax -1 - i) * 255/savemax;
            ofSetColor(col);
            cap[num][capCount].draw(0,0);
        }
    }
}

void ofApp::drawPastBack(){
    ofSetColor(colSld);
    
    backPastCount--;
    if(backPastCount <= -1){
        backPastCount = capmax-1;
        
        if(backPastNum == 0){
            backPastNum = savemax-1;
        }else{
            backPastNum--;
        }
    }
    if(capDam[backPastNum][backPastCount].bAllocated()){
        capDam[backPastNum][backPastCount].draw(camWidth*2,0);
    }
}

void ofApp::drawPast(){
    ofSetColor(colSld);
    if(capDam[pastNum][pastCount].bAllocated()){
        capDam[pastNum][pastCount].draw(camWidth,0);
        pastCount++;
    }else if(pastCount >= capmax){
        capDam[pastNum][capmax-1].draw(camWidth, 0);
    }
    if(pastCount >= capmax){
        pastCount = 0;
        if(pastNum == savemax-1){
            pastNum = 0;
        }else{
            pastNum++;
        }
    }
}

void ofApp::drawLocus(){
    int num = capNum;
    locusCol = colSld;
    for(int i=0; i<savemax; i++){
        num--;
        if(num < 0){num = savemax-1;}
        if(cap[num][capmax-1].bAllocated()){
            locusCol.a = (savemax -1 - i) * 255/savemax;
            ofSetColor(locusCol);
            cap[num][capmax-1].draw(0,0);
        }
    }
}

void ofApp::drawSub(){
    ofSetColor(colSld);
    if (cap[capNum][capCount].bAllocated()) {
        // 過去の映像表示
        for(int i=0; i<savemax/5; i++){
            for(int j=0; j<5; j++){
                if(cap[i*5+j][capCount].bAllocated()){
                    cap[i*5+j][capCount].draw(camWidth/5*j, camHeight+camHeight/5*i, camWidth/5, camHeight/5);
                }
            }
        }
    }
}

void ofApp::drawRandom(){
    if(random){
        randPos.x = ofRandom(camWidth/4, camWidth/4*3);
        randPos.y = ofRandom(camHeight/4,camHeight/4*3);
        randScale = ofRandom(0,1);
    }
    cap[0][capCount].draw(randPos.x-camWidth*randScale/2,randPos.y-camWidth*randScale/2, camWidth*randScale, camHeight*randScale);
}

void ofApp::drawRandomCap(){
    //ランダムな画像を描画
    int num = ofRandom(SAVEMAX-1);
    int count = ofRandom(CAPMAX-1);
    if(capRegular[num][count].bAllocated()){capRegular[num][count].draw(0,0);}
}

void ofApp::drawRegular(){
    ofSetColor(255);
    for(int i=0; i<regularSld; i++){
        if(capRegular[regularRandNum[i]][capCount].bAllocated()){
            capRegular[regularRandNum[i]][capCount].draw(0,camHeight);
        }
    }
}

void ofApp::presResetbutton(){
    for(int i=0; i<COLNUM; i++){
        eraseColor[i] = ofColor(0,100+i*5,0);
        drawColor[i]  = ofColor(0,0,0);
        
        for(int i=0; i<SAVEMAX; i++){
            for(int j=0; j<CAPMAX; j++){
                cap[i][j].clear();
                capDam[i][j].clear();
                capRegular[i][j].clear();
            }
        }
        savemax = saveSld;
        capmax = capSld;
        capCount        = 0;
        capNum          = 0;
        eraseId         = 0;
        drawId          = 0;
        copyCapNum      = 0;
        copyCapCount    = 0;
        backPastNum     = 0;
        backPastCount   = 0;
        pastNum         = 0;
        pastCount       = 0;
        clickFlg        = true;
        threshold       = 30;
    }

}

void ofApp::drawFlowCap(){
    
    int flame = capNum*capmax + capCount;
    int flameMax = savemax*capmax;
    
    ofSetColor(255);
    
    
    
    for(int i=0; i<flameMax; i++){
        if(flowValue[i] > 8){
            if(capFlow[i].bAllocated()){
                ofSetColor(255,255,255,ofMap(i, 0, flameMax, 0, 255));
                capFlow[i].draw(0,0);
            }
        }
    }
    
    
    
//    for(int i=0; i<flameMax; i++){
//        if(flowDrawflg[i]){flowDrawflg[i] = false;}
//        
//        if(flowWaitFlame <= 0 || (flowWaitFlame > 10 && flowValue[i] > 5)){
//            //描画したら待ち時間増加
//            //動きの小さい時に待ち時間多く、大きい時に待ち時間少なく
//            if(abs(flowValue[i]) > 5){
//                flowWaitFlame+= 5;
//            }else if(abs(flowValue[i]) > 3){
//                flowWaitFlame += 10;
//            }else if(abs(flowValue[i]) >= 0){
//                flowWaitFlame += 20;
//            }
//            
//            flowDrawflg[i] = true;
//        }
//        ofDrawBitmapStringHighlight("waitFlame" + ofToString(flowWaitFlame), 0, camHeight+20);
//        flowWaitFlame--;
//    }
//    
//    
//    for(int k=0; k<flameMax; k++){
//    
//        if(flowDrawflg[k]){
//            if(capFlow[k].bAllocated()){
//                    capFlow[k].draw(0,camHeight);
//                    //cout << flame+k <<endl;
//               }
//        }
//    }
    
    
//    if(flameMax - flowValue[flameMax-1] >= 0){
//        for (int i = flameMax - flowValue[flameMax-1]; i <= flameMax; i++) {
//            if(capFlow[i-1].bAllocated()){
//                capFlow[i-1].draw(0,camHeight);
//            }
//        }
//        cout << flameMax - flowValue[flameMax-1] << endl;
//    }
//    
    for(int i=0; i<flameMax; i++){
    
    
    
    
//        if(i + capCount >= flameMax){
//            capFlow[].draw(camWidth,camHeight);
//            i+=3;
//        }else {
//            capFlow[i +capCount].draw(camWidth,camHeight);
//            i+=3;
//        }

//        if(flowValue[i] > 7){
//            i+=1;
//        }else if(flowValue[i] > 5){
//            i+=3;
//        }else if (flowValue[i] > 3){
//            i += 5;
//        }else if(flowValue[i] > 1){
//            i += 8;
//        }else{i += 10;}

//        if(capFlow[i].bAllocated()){
//            capFlow[i].draw(0,camHeight);
//        }
//
//        i += 15 - flowValue[flameMax-1];
//    

    
//        if(abs(flowValue[i]) > 7){
//            if(i + capCount >= flameMax){
//                capFlow[i+capCount-flameMax].draw(camWidth,camHeight);
//                i+=3;
//            }else {
//                capFlow[i +capCount].draw(camWidth,camHeight);
//                i+=3;
//            }
//        }
//        
//        else if(abs(flowValue[i]) > 5){
//            if(i + capCount >= flameMax){
//                capFlow[i+capCount-flameMax].draw(camWidth,camHeight);
//                i+=5;
//            }else {
//                capFlow[i +capCount].draw(camWidth,camHeight);
//                i+=5;
//            }
//        }
//        
//        else if(abs(flowValue[i]) > 3){
//            if(i + capCount >= flameMax){
//                capFlow[i+capCount-flameMax].draw(camWidth,camHeight);
//                i+=10;
//            }else {
//                capFlow[i +capCount].draw(camWidth,camHeight);
//                i+=10;
//            }
//        }
//        
//        else if(abs(flowValue[i]) >= 0){
//            if(i + capCount >= flameMax){
//                capFlow[i+capCount-flameMax].draw(camWidth,camHeight);
//                i+=20;
//            }else {
//                capFlow[i +capCount].draw(camWidth,camHeight);
//                i+=20;
//            }
//        }
    }
}


void ofApp::drawEffect(){


    if(flowAve < 5){
        changeCol = 0;
    }else if(flowAve < 15){
        changeCol = 2;
    }else if (flowAve < 25){
        changeCol = 3;
    }else{
        changeCol = 1;
    }
    ofSetColor(0);
    for(int i=0; i<100; i++){
    ofSetColor(particleCol[i]);
        particle[i].x += particleVec[i].x;
        particle[i].y += particleVec[i].y;
        if(particle[i].x > camWidth || particle[i].x < 0){particle[i].x = camWidth/2;particle[i].y = camHeight/2;}
        if(particle[i].y > camHeight || particle[i].y < 0){particle[i].y = camHeight/2;particle[i].x = camWidth/2;}
        ofCircle(particle[i].x, particle[i].y,particle[i].z);
    }
}