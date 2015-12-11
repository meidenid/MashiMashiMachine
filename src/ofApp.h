#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    unsigned char* onAlpha(unsigned char* cp);
    void drawCol();
    
    //過去の映像を描画
    void drawCap();
    
    //'c'を押した時点の映像を逆再生
    void drawPastBack();
    
    //'c'を押した時点の映像を再生
    void drawPast();
    
    //過去の静止画を描画
    void drawLocus();
    
    //cap配列を描画(小さく５個並んでる奴)
    void drawSub();
    
    //ランダムに選択した画像を描画
    void drawRandomCap();
    
    //ランダムな位置と大きさに描画
    void drawRandom();
    
    void drawRegular();
    
    void presResetbutton();
    
    void drawFlowCap();

    //えふぇくつ
    void drawEffect();

    //背景差分
    void bgSubtraction();
    
    void drawEffect();
    
    
    ofImage  img;
    
    int capmax = 10;
    int savemax = 5;
    static const int CAPMAX = 30;
    static const int SAVEMAX = 20;
    static const int COLNUM = 10;
    static const int REGMAX = 30;
    ofVideoGrabber  vidGrabber;
    ofTexture       videoTexture;
    int             camWidth;
    int             camHeight;
    bool            capFlg;
    bool            regularFlg;
    int             capCount;
    int             capNum;
    int             copyCapNum;
    int             copyCapCount;
    int             pastCount;
    int             pastNum;
    int             backPastNum;
    int             backPastCount;
    int             regularNum;
    int             flowValue[SAVEMAX*CAPMAX];
    bool            flowDrawflg[SAVEMAX*CAPMAX];
    int             regularRandNum[REGMAX];
    int             flowWaitFlame;
    bool            clickFlg;
    float           flowAve;
    
    bool            isCapFlowFilled;
    
    bool            speedFlg;
    ofImage         cap[SAVEMAX][CAPMAX];
    ofImage         capDam[SAVEMAX][CAPMAX];
    ofImage         capRegular[REGMAX][CAPMAX];
    ofImage         capFlow[SAVEMAX*CAPMAX];
    ofImage         sato;
    ofColor         locusCol;
    ofColor         eraseColor[COLNUM];
    ofColor         drawColor[COLNUM];
    int             eraseId;
    int             drawId;
    int             threshold;
    float           randScale;
    float           speedScale;
    ofPoint         randPos;
    
    //はいけいさぶん
    bool            bLearnBakground;
    ofImage         bgImage;
    ofImage         prevDiffImg;
    ofImage         diffImg;
    ofxCvGrayscaleImage diffGrayImg;
    ofxCvColorImage     diffColorImg;
    cv::Scalar      diffMean;
    
    //Effect
    ofxCv::ContourFinder myContourFinder;
    ofxCvContourFinder myCountourFinder2;

    
    ofxPanel        gui;
    ofxIntSlider    capSld;
    ofxIntSlider    saveSld;
    ofxIntSlider    threSld;
    ofxIntSlider    regularSld;
    ofxFloatSlider  speed;
    ofxColorSlider  colSld;
    ofxIntSlider    changeCol;
    ofxButton       random;
    ofxButton       reset;
    ofxButton       captur;
    ofxToggle       reguner;
    ofxToggle       drawpast;
    ofxToggle       drawpastBack;
    ofxToggle       mode;
    ofPoint         particle[100];
    ofColor         particleCol[100];
    ofPoint         particleVec[100];
    ofVec2f         pyrAve;
    
    ofxPanel        cv;
    ofxFloatSlider  pyrScale;
    ofxIntSlider    level;
    ofxIntSlider    winsize;
    ofxIntSlider    iteration;
    ofxIntSlider    polyN;
    ofxFloatSlider  polySigma;
    ofxIntSlider    winSize;
    ofxIntSlider    maxLevel;
    ofxIntSlider    coutourThresholded;
    ofxIntSlider    contourMinArea;
    ofxFloatSlider  maxFeatures;
    ofxFloatSlider  qualityLevel;
    ofxFloatSlider  minDistance;
    
    ofxToggle       OPTFLOW;
    ofxToggle       useFarneback;
    
    
    
    ofxCv::ContourFinder contourFinder;
    ofxCv::RunningBackground background;
    ofImage thresholded;
    
    ofxCv::FlowFarneback farneback;
    ofxCv::FlowPyrLK pyrLk;
    ofxCv::Flow* curFlow;
    
    ofVec2f flowAve;
    ofVec2f pyrAve;
};
