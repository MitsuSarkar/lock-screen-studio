#include "../src/widget-placement.h"
#include <cassert>
#include <iostream>
#include <random>
int main() {
    std::mt19937 generator(4096);
    std::uniform_real_distribution<double> values(-10000,10000);
    // Property checks cover screen sizes/DPI, existing positions, and offsets
    // far beyond either edge. Validate bounds and idempotence after refresh.
    for (int i=0;i<20000;++i) {
        double viewport=320+std::abs(values(generator));
        double padding=std::abs(values(generator))/100;
        double extent=20+std::abs(values(generator))/3;
        double start=values(generator), requested=values(generator);
        double result=MitsuPlacement::ClampOffset(start,extent,viewport,padding,requested);
        assert(std::isfinite(result));
        if (extent+2*padding<=viewport) {
            assert(start+result>=padding-1e-8);
            assert(start+result+extent<=viewport-padding+1e-8);
        } else assert(std::abs(start+result-(viewport-extent)/2)<1e-8);
        double again=MitsuPlacement::ClampOffset((start+result)-result,extent,viewport,padding,requested);
        assert(std::abs(again-result)<1e-8);
    }
    assert(MitsuPlacement::ClampOffset(100,300,1920,16,0)==0);
    assert(MitsuPlacement::ClampOffset(100,0,1920,16,1000)==0);
    // Regression: a card at x=1010 is inside a parent viewport [950,1350].
    // A -300 request fits the display but leaves only its right edge visible.
    // Retained shared viewports must clamp against that parent, not the display.
    auto clipped=MitsuPlacement::KeepVisible(1010-950,100,320,200,400,800,16,{0,0},{-300,0},{1,0},{0,1});
    assert(std::abs(clipped.x-(-44))<1e-8);
    assert(1010+clipped.x>=950+16);
    auto repeated=MitsuPlacement::KeepVisible(1010+clipped.x-950,100,320,200,400,800,16,clipped,{-300,0},{1,0},{0,1});
    assert(std::abs(repeated.x-clipped.x)<1e-8);
    // Once the whole widget-only viewport moves together, the display is the
    // constraint and the full -300 offset is valid without internal clipping.
    auto whole=MitsuPlacement::KeepVisible(1010,100,320,200,1920,1080,16,{0,0},{-300,0},{1,0},{0,1});
    assert(whole.x==-300);
    // Studio mode has its own 320-DIP card in the root, not a full-width native
    // row. A -300 request must remain a -300 movement when it fits the screen.
    assert(MitsuPlacement::ClampOffset((1920-320)/2,320,1920,16,-300)==-300);
    for (int i=0;i<10000;++i) {
        double sx=0.5+std::abs(values(generator))/4000,sy=0.5+std::abs(values(generator))/4000;
        MitsuPlacement::Offset previous{values(generator),values(generator)},request{values(generator),values(generator)};
        double left=100+previous.x*sx,top=200+previous.y*sy;
        auto actual=MitsuPlacement::KeepVisible(left,top,250,200,1920,1080,16,previous,request,{sx,0},{0,sy});
        assert(100+actual.x*sx>=16-1e-8 && 100+actual.x*sx+250<=1904+1e-8);
        assert(200+actual.y*sy>=16-1e-8 && 200+actual.y*sy+200<=1064+1e-8);
        auto again=MitsuPlacement::KeepVisible(100+actual.x*sx,200+actual.y*sy,250,200,1920,1080,16,actual,request,{sx,0},{0,sy});
        assert(std::abs(again.x-actual.x)<1e-8 && std::abs(again.y-actual.y)<1e-8);
    }
    std::cout << "30,000 placement cases plus narrow-parent -300 clipping regressions passed.\n";
}
