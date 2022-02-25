#define DYNA_MAXWIDTH   (uiinchtopix(2.0))

// DynamicObject
//
//  DynamicObject()
//      void moveto(Point p)
//      void lineto(Point p)
//      void lineto()
//      void endpath()
//      void setwidth(float _width)
//      Point diagoffset(Point p, float delta)
//      Rect draw()
//
class DynamicObject {
  public:
    float mass = 2000.0;
    float drag = 0.008;
    Point touchPos;
    Point lastPos, lastVel;
    Point CurPos, CurVel;
    float width = DYNA_MAXWIDTH/2.0;
    float lastWidth = DYNA_MAXWIDTH/2.0;
    Point lastDrawPos;
    bool drawing = false;
    Path strokep;

    DynamicObject() {
    }
    void clear() {
        strokep = Path();
    }
    Path getpath() {
        return strokep;
    }
    void moveto(Point p) {
        touchPos = p;
        lastPos = p;
        lastVel = Point(0.0, 0.0);
        CurPos = p;
        CurVel = Point(0.0, 0.0);
        lastDrawPos = p;
        drawing = true;
    }
    void lineto(Point p) {
        touchPos = p;
        Point a = (touchPos - CurPos)/mass;
        lastPos = CurPos;
        lastVel = CurVel;
        CurVel += a;
        CurVel *= (1.0-drag);
        CurPos += CurVel;
    }
    void lineto() {
        lineto(touchPos);
    }
    void endpath() {
        drawing = false;
    }
    void setwidth(float _width) {
        width = DYNA_MAXWIDTH*_width;
    }
    Point diagoffset(Point p, float delta) {
        float a = 60.0*(Math.PI/180.0);
        return p+Point(delta*cosf(a),delta*sinf(a));
    }
    Rect draw() {
        if (lastDrawPos == CurPos) {
            return RectNull();
        } else {
            Path p;
            Point pos;
            p.moveto(diagoffset(lastDrawPos, -lastWidth/2.0));
            p.lineto(diagoffset(CurPos,          -width/2.0));
            p.lineto(diagoffset(CurPos,           width/2.0));
            p.lineto(diagoffset(lastDrawPos,  lastWidth/2.0));
            p.closepath();
            p.setsimple();
            if (p.area()<0.0)
                p.evert();
            p.fill();
            p.stroke(0.1*uiinchtopix(0.01));
            strokep.append(p);
            lastWidth = width;
            lastDrawPos = CurPos;
            return p.rect;
        }
    }
};

#define CALC_RES        (2)

// uvDynaDrawApp
//
//  uvDynaDrawApp(const char *name) 
//
class uvDynaDrawApp: public uv {
  public:
    Path p;
    uvButton *buttonClear;
    uvSlider *sizeSlider;
    DynamicObject *dynaObj;
    float curBlend;
    double lastSec = 0.0;

    uvDynaDrawApp(const char *_name) {
        setclassname("uvDynaDrawApp");
        setname(_name);

        subviewadd(sizeSlider = new uvSlider("sizeSlider", uiinchtopix(4.0)));
        dynaObj = new DynamicObject();
        subviewadd(buttonClear = new uvButton("clear", "gen/img_button_blank.raw"));
        settransparent(true);
    }
    void _drawSegment() {
        gl.colorf(0.0, 0.0, 0.0, 1.0);
        dodraw(dynaObj->draw());
    }
    bool event(const char *msg) {
        if (strequal(msg, "sizeSlider.move")) {
            float width = eventslidervalue();
            dynaObj->setwidth(width);
            return true;
        }
        if (strequal(msg, "clear.doit")) {
            dynaObj->clear();
            curBlend = 0.0;
            animate(1.0);
            return true;
        }
        return false;
    }
    void animwork(float param) {
        backfbo.bind();
        gl.initstate();
        gl.colorwhite(sqrt(param));
        rect.fill();
        dodraw();
    }
    uv *click1(Point pos) {
        lastSec = Utils.getdnow();
        dynaObj->moveto(pos);
        timerset(0.0);
        if (anim)
            anim->stop();
        return this;
    }
    void move1(Point pos0, Point pos1) {
        backfbo.bind();
        gl.initstate();
        dynaObj->lineto(pos1);
        _drawSegment();
    }
    void unclick1(Point pos) {
        dynaObj->endpath();
    }
    void timer() {
        if (dynaObj->drawing) {
            backfbo.bind();
            gl.initstate();
            double now = Utils.getdnow();
            int deltaT = round(1000.0*(now-lastSec));
            int repCount = round(deltaT/CALC_RES);
            if (repCount>100) repCount = 100;
            if (repCount==0) 
                return;
            else
                lastSec = now;
            for(int rep = 0; rep<repCount; rep += repCount/2) {
                for(int i=0; i<rep; i++)
                    dynaObj->lineto();
                _drawSegment();
            }
            timerset(0.0);
        }
    }

    void layout(Rect r) {
        setrect(r);
        if (backfbo.rect != rect) {
            backfbo = FBO(rect, WANT_TEXTUREMAP);
            backfbo.setname("DynaDraw canvas");
            backfbo.bind();
            gl.initstate();
            gl.clearcolor(255, 255, 255, 255);
            gl.clearscreen();
        }
        r = sizeSlider->measure();
        r = RectCenterInRect(rect, r);
        r.origin.y = uiinchtopix(0.4);
        sizeSlider->layout(r);
        r = buttonClear->measure();
        r.origin = Point(uiinchtopix(0.4), uiinchtopix(0.4));
        buttonClear->layout(r);
        bool vis = rect.minsize() > uiinchtopix(1.0);
        sizeSlider->setvisible(vis);
        buttonClear->setvisible(vis);
    }
    void drawview() {
        gl.colorwhite();
        backfbo.draw();
    }
};
