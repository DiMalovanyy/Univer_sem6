#include "wx/wx.h"
#include "wx/sizer.h"
#include <vector>
#include <algorithm>


#include "compute.h"

#define POINT_WIDTH 10

class BasicDrawPane : public wxPanel {
    
public:
    BasicDrawPane(wxFrame* parent);
    
    void paintEvent(wxPaintEvent & evt);
	void mouseDown(wxMouseEvent& event);
    
    DECLARE_EVENT_TABLE()
	std::vector<wxPoint> points;        
};


class MyApp: public wxApp
{
    bool OnInit();
    
    wxFrame *frame;
    BasicDrawPane * drawPane;
public:
};

IMPLEMENT_APP(MyApp)


bool MyApp::OnInit() {
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    frame = new wxFrame((wxFrame *)NULL, -1,  wxT("OGTG Lab Dmytro Malovanyi"), wxPoint(50,50), wxSize(800,600));
	
    drawPane = new BasicDrawPane( (wxFrame*) frame );
    sizer->Add(drawPane, 1, wxEXPAND);
	
    frame->SetSizer(sizer);
    frame->SetAutoLayout(true);
	
    frame->Show();
    return true;
} 

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
EVT_PAINT(BasicDrawPane::paintEvent)
EVT_LEFT_DOWN(BasicDrawPane::mouseDown)
END_EVENT_TABLE()

BasicDrawPane::BasicDrawPane(wxFrame* parent) :
wxPanel(parent) {}

void BasicDrawPane::paintEvent(wxPaintEvent & evt) {
    wxPaintDC dc(this);
	if (points.size() >= 3) {
		Circle min_circle = makeSmallestEnclosingCircle(convertFromWxPoint(points));
		dc.SetPen( wxPen( wxColor(0,0,255), 5));
		dc.DrawCircle( wxPoint(min_circle.c.x, min_circle.c.y), min_circle.r);

	}
	dc.SetPen( wxPen( wxColor(255,0,0), POINT_WIDTH )); 
	for(const wxPoint point: points) {
		dc.DrawPoint(point);
	}
}

void BasicDrawPane::mouseDown(wxMouseEvent& event) {
	wxPoint current_point = wxGetMousePosition() - GetScreenPosition();

	auto remove_start = std::remove_if(points.begin(), points.end(), [current_point](wxPoint point) {
		return pow(current_point.x - point.x, 2) + pow(current_point.y - point.y, 2) < pow(POINT_WIDTH/2, 2);
	});

	if (remove_start != points.end()) {
		points.erase(remove_start, points.end());
	} else {
		points.push_back(current_point);
	}
	Refresh();
}
