// Pure placement policy, also exercised by tools/test-widget-placement.cpp.
#include <algorithm>
#include <cmath>
namespace MitsuPlacement {
inline double ClampOffset(double start, double extent, double viewport, double padding, double requested) {
    if (!std::isfinite(start) || !std::isfinite(extent) || !std::isfinite(viewport) ||
        !std::isfinite(requested) || extent <= 0 || viewport <= 0) return 0;
    padding = (std::clamp)(padding, 0.0, viewport / 2.0);
    double low = padding - start;
    double high = viewport - padding - extent - start;
    // If the entire row cannot fit, center it so both ends remain reachable.
    if (low > high) return (viewport - extent) / 2.0 - start;
    return (std::clamp)(requested, low, high);
}
struct Offset { double x,y; };
inline Offset KeepVisible(double left,double top,double width,double height,
    double viewportWidth,double viewportHeight,double padding,Offset previous,
    Offset requested,Offset xAxis,Offset yAxis) {
    double determinant=xAxis.x*yAxis.y-yAxis.x*xAxis.y;
    if (!std::isfinite(determinant) || std::abs(determinant)<1e-6) return {0,0};
    double baseX=left-previous.x*xAxis.x-previous.y*yAxis.x;
    double baseY=top-previous.x*xAxis.y-previous.y*yAxis.y;
    double x=ClampOffset(baseX,width,viewportWidth,padding,requested.x*xAxis.x+requested.y*yAxis.x);
    double y=ClampOffset(baseY,height,viewportHeight,padding,requested.x*xAxis.y+requested.y*yAxis.y);
    return {(x*yAxis.y-y*yAxis.x)/determinant,(y*xAxis.x-x*xAxis.y)/determinant};
}
}
