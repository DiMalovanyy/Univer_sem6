#pragma once

#include <vector>
#include <wx/gdicmn.h>

struct Point final {
public:
	double x;
	double y;
	
	
	Point subtract(const Point &p) const;
	
	double distance(const Point &p) const;
	
	// Signed area / determinant thing
	double cross(const Point &p) const;
};


struct Circle final {
public:	
	static const Circle INVALID;
	
	static const double MULTIPLICATIVE_EPSILON;
	
	Point c;   // Center
	double r;  // Radius
	
	bool contains(const Point &p) const;
	bool contains(const std::vector<Point> &ps) const;
};

static std::vector<Point> convertFromWxPoint(const std::vector<wxPoint>& points) {
	std::vector<Point> return_pt;
	for(const wxPoint& point: points) {
		Point pt;
		pt.x = point.x;
		pt.y = point.y;
		return_pt.push_back(pt);
	}
	return return_pt;
}


Circle makeSmallestEnclosingCircle(std::vector<Point> points);

Circle makeDiameter(const Point &a, const Point &b);
Circle makeCircumcircle(const Point &a, const Point &b, const Point &c);
