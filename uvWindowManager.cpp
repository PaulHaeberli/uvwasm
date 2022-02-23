#define DRAG_NONE       (0)
#define DRAG_TRANSLATE  (1)
#define DRAG_LL         (2)
#define DRAG_LR         (3)

#define MODE_FIXEDSIZE     (1)
#define MODE_VARIABLESIZE  (2)

// uvWindow
//
//  uvWindow(const char *name, uv *v)
//  uvWindow(const char *name, uv *v, const char *title)
//      void settopview(bool topview)
//      void dodrawrect()
//      void layouthint(int pos, int nviews)
//
class uvWindow: public uv {
  public:
    uv *v;
    int mode;
    bool forcetop;
    uvLabel *label;
    bool topview = false;
    int dragging = 0;
    Rect subrect;
    Rect outerrect;
    float layoutpos = 0.0;
    int titlesizey;
    UIBlobs blobs;

    uvWindow(const char *_name, uv *_v) {
        _doinit(_name, _v, "");
    }
    uvWindow(const char *_name, uv *_v, const char *_title) {
        _doinit(_name, _v, _title);
    }
    void _doinit(const char *_name, uv *_v, const char *_title) {
        setclassname("uvWindow");
        setname(_name);

        v = _v;
        setname(v->name);
        subviewadd(v);
        if (strlen(_title) == 0) {
            mode = MODE_FIXEDSIZE;
            forcetop = true;
            label = NULL;
        } else {
            mode = MODE_VARIABLESIZE;
            forcetop = false;
            label = new uvLabel(_title, style.PopupLabel, _title);
            subviewadd(label);
        }
    }
    void _setdrag(int drag) {
        dragging = drag;
    }
    void settopview(bool _topview) {
        topview = _topview;
    }
    void dodrawrect() {
        dodraw(outerrect);
    }
    uv *click1(Point pos) {
        if (mode == MODE_VARIABLESIZE) {
            if (PointDistance(pos, subrect.ll()) < uiinchtopix(0.15)) {
                sendevent("pop");
                _setdrag(DRAG_LL);
                return this;
            }
            if (PointDistance(pos, subrect.lr()) < uiinchtopix(0.15)) {
                sendevent("pop");
                _setdrag(DRAG_LR);
                return this;
            }
        }
        if (subrect.clone().expand(uiinchtopix(0.10)).containspoint(pos)) {
            sendevent("pop");
            _setdrag(DRAG_TRANSLATE);
            return this;
        } else {
            _setdrag(DRAG_NONE);
            return 0;
        }
    }
    void move1(Point pos0, Point pos1) {
        Point delta = PointVect(pos1, pos0);
        switch (dragging) {
            case DRAG_TRANSLATE:
                dodrawrect();
                subrect.translate(delta).round();
                _dolayout();
                dodrawrect();
                break;
            case DRAG_LL:
                dodrawrect();
                subrect.origin.x += delta.x;
                subrect.origin.y += delta.y;
                subrect.size.x -= delta.x;
                subrect.size.y -= delta.y;
                _dolayout();
                dodrawrect();
                break;
            case DRAG_LR:
                dodrawrect();
                subrect.origin.y += delta.y;
                subrect.size.x += delta.x;
                subrect.size.y -= delta.y;
                _dolayout();
                dodrawrect();
                break;
            default:
                break;
        }
    }
    void unclick1(Point pos) {
        if (dragging == DRAG_NONE)
            return;
        _setdrag(DRAG_NONE);
        dodrawrect();
    }
    void _dolayout() {
        outerrect = subrect;
        outerrect.expandbot(uiinchtopix(0.10));
        outerrect.expandleft(2*params.uilinewidth);
        outerrect.expandright(2*params.uilinewidth);
        Rect r, lr;
        switch (mode) {
            case MODE_VARIABLESIZE:
                lr = label->measure();
                titlesizey = 1.5*lr.size.y;
                lr.origin = subrect.origin + Point(round(0.3*lr.size.y), subrect.size.y - titlesizey + uiinchtopix(0.015));
                label->layout(lr);
                r = subrect;
                v->layout(r.insettop(titlesizey));
                break;
            case MODE_FIXEDSIZE:
                r = v->measure().translate(subrect.origin);
                v->layout(r);
                break;
            default:
                break;
        }
    }
    void layouthint(int pos, int nviews) {
        if (nviews <= 1)
            layoutpos = 0.5;
        else
            layoutpos = pos/(nviews-1.0);
    }
    void layout(Rect r) {
        setrect(r);
        Point p0, p1;
        int delta;
        fprintf(stderr, "uvWindow: layout\n");
        switch (mode) {
            case MODE_VARIABLESIZE:
                p0 = Point( rect.size.x, -rect.size.y) * 0.15;
                p1 = Point(-rect.size.x,  rect.size.y) * 0.15;
                subrect = r.centerscale(0.5).translate(PointLerp(p0, p1, layoutpos));
                break;
            case MODE_FIXEDSIZE:
                delta = uiinchtopix(0.1);
                subrect = v->measure();
                subrect.origin += Point(delta, delta);
                break;
            default:
                break;
        }
        _dolayout();
    }
    UIBlobs borderrects() {
        UIBlobs blobs;

        float a = 0.75;
        int shade;
        if (dragging == DRAG_NONE) {
            if (topview)
                shade = 246;
            else
                shade = 236;
        } else {
            shade = 200;
        }

        Rect dr = outerrect;
        dr.origin = Point(0,0);
        dr.inset(params.uilinewidth);
        blobs.setrect(outerrect);
        blobs.setrad(params.uirectrad-0*params.uilinewidth);
        blobs.addrectangle(dr, Color(0, 0, 0, 40));

        dr.inset(params.uilinewidth);
        blobs.setrad(params.uirectrad-1*params.uilinewidth);
        blobs.addrectangle(dr, Color(a*shade, a*shade, a*shade, a*255));

        return blobs;
    }
    void drawview() {
        fprintf(stderr, "uvWindow: drawview\n");
        clearview();
        gl.colorwhite();
        borderrects().draw();
    }
};

// uvWindowManager - manage a bunch of WindowViews
//
//  uvWindowManager(const char *name) {
//      void addview(uv *v) 
//
class uvWindowManager: public uv {
  public:

    uvWindowManager(const char *_name) {
        setclassname("uvWindowManager");
        setname(_name);
    }
    void addview(uv *v) {
        subviewadd(new uvWindow(v->name, v, v->name));
    }
    bool event(const char *msg) {
        if (strequal(msg, "pop")) {
            for (int i=0; i<subviews.size(); i++) {
                uvWindow *wv = (uvWindow *)subviews[i];
                wv->settopview(false);
            }
            uvWindow *wv = (uvWindow *)eventsource();
            assert(wv);
            subviewremove(wv);
            uvWindow *oldtop = (uvWindow *)subviews[subviews.size()-1];
            subviewaddabove(wv, oldtop);
            wv->settopview(true);
            wv->dodrawrect();
            oldtop->dodrawrect();
            return true;
        }
        return false;
    }
    void layout(Rect r) {
        setrect(r);
        int nresizable = 0;
        for (int i=0; i<subviews.size(); i++) {
            uvWindow *wv = (uvWindow *)subviews[i];
            if (wv->mode == MODE_VARIABLESIZE)
                nresizable++;
        }
        int pos = 0;
        for (int i=0; i<subviews.size(); i++) {
            uvWindow *wv = (uvWindow *)subviews[i];
            if (wv->mode == MODE_VARIABLESIZE) {
                wv->layouthint(pos, nresizable);
                pos += 1;
            }
            wv->layout(r);
        }
    }
    void drawview() {
        clearview();
    }
};

class uvMultiApp: public uv {
  public:
    uvMultiApp(const char *name) {
        setclassname("uvMultiApp");
        setname(name);
        
        uvWindowManager *wm;
        subviewadd(wm = new uvWindowManager("TheWindowManager"));
        wm->setbgcolor(Color(200, 200, 210, 255));
        wm->addview(new uvHideLineApp("HideLine"));
        wm->addview(new uvDynaDrawApp("DynaDraw"));
        wm->addview(trackballscene2(new uv3ring("ring")));
    }
};
