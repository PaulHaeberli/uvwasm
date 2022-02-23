#define CALC_RES        (2)
#define DYNA_MAXWIDTH   (60.0)

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
    Point touch_pos;
    Point last_pos;
    Point last_vel;
    Point cur_pos;
    Point cur_vel;
    double last_sec = 0.0;
    float width = DYNA_MAXWIDTH/2.0;
    float last_width = DYNA_MAXWIDTH/2.0;
    Point last_drawpos;
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
        touch_pos = p;
        last_pos = p;
        last_vel = Point(0.0, 0.0);
        cur_pos = p;
        cur_vel = Point(0.0, 0.0);
        last_drawpos = p;
        last_sec = Utils.getdnow();
        drawing = true;
    }
    void lineto(Point p) {
        touch_pos = p;
        double now = Utils.getdnow();
        int deltat = round(1000.0*(now-last_sec));
        int repcount = round(deltat/CALC_RES);
        if (repcount>100)
            repcount = 100;
        if (repcount==0) 
            return;
        else
            last_sec = now;
        for (int i=0; i<repcount; i++) {
            Point a = (touch_pos - cur_pos)/mass;
            last_pos = cur_pos;
            last_vel = cur_vel;
            cur_vel += a;
            cur_vel *= (1.0-drag);
            cur_pos += cur_vel;
        }
    }
    void lineto() {
        lineto(touch_pos);
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
        if (last_drawpos == cur_pos) {
            return RectNull();
        } else {
            Path p;
            Point pos;
            p.moveto(diagoffset(last_drawpos, -last_width/2.0));
            p.lineto(diagoffset(cur_pos,           -width/2.0));
            p.lineto(diagoffset(cur_pos,            width/2.0));
            p.lineto(diagoffset(last_drawpos,  last_width/2.0));
            p.closepath();
            p.setsimple();
            if (p.area()<0.0)
                p.evert();
            p.fill();
            p.stroke(0.1*uiinchtopix(0.01));
            strokep.append(p);
            last_width = width;
            last_drawpos = cur_pos;
            return p.rect;
        }
    }
};

// uvDynaDrawApp
//
//  uvDynaDrawApp(const char *name) 
//
class uvDynaDrawApp: public uv {
  public:
    Path p;
    uvButton *button_clear;
    uvSlider *sizeslider;
    DynamicObject *dynaobj;
    float cur_blend;

    uvDynaDrawApp(const char *_name) {
        setclassname("uvDynaDrawApp");
        setname(_name);

        subviewadd(sizeslider = new uvSlider("sizeslider", uiinchtopix(4.0)));
        dynaobj = new DynamicObject();
        subviewadd(button_clear = new uvButton("clear", "gen/img_button_blank.raw"));
        settransparent(true);
    }
    ~uvDynaDrawApp() {
        fprintf(stderr, "DynaDrawApp: DEINIT\n");
    }
    void _drawsegment() {
        gl.colorf(0.0, 0.0, 0.0, 1.0);
        dodraw(dynaobj->draw());
    }
    bool event(const char *msg) {
        if (strequal(msg, "sizeslider.move")) {
            float width = eventuv(uvSlider)->valueget();
            dynaobj->setwidth(width);
            return true;
        }
        if (strequal(msg, "clear.doit")) {
            dynaobj->clear();
            cur_blend = 0.0;
            animate(1.0);
            return true;
        }
        return false;
    }
    void animwork(float param) {
        float a = sqrt(param);
        backfbo.bind();
        gl.initstate();
        gl.colorwhite(a);
        rect.fill();
        dodraw();
    }
    uv *click1(Point pos) {
        dynaobj->moveto(pos);
        timerset(0.0);
        if (anim)
            anim->stop();
        return this;
    }
    void move1(Point pos0, Point pos1) {
        backfbo.bind();
        gl.initstate();
        dynaobj->lineto(pos1);
        _drawsegment();
    }
    void unclick1(Point pos) {
        dynaobj->endpath();
    }

    void timer() {
        backfbo.bind();
        gl.initstate();
        if (dynaobj->drawing) {
            dynaobj->lineto();
            _drawsegment();
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
        r = sizeslider->measure();
        r = RectCenterInRect(rect, r);
        r.origin.y = r.size.y;
        sizeslider->layout(r);
        r = button_clear->measure();
        r.origin = Point(20+0.0, 20+0.0);
        button_clear->layout(r);
        bool vis = rect.minsize() > uiinchtopix(1.0);
        sizeslider->setvisible(vis);
        button_clear->setvisible(vis);
    }
    void drawview() {
        gl.colorwhite();
        backfbo.draw();
    }
};
