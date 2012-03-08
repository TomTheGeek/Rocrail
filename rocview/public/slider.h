/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef SLIDER_H_
#define SLIDER_H_

class Slider : public wxPanel
{
    wxPanel* Parent;
    int Value;
    int Min;
    int Max;
    int Width;
    int Height;
    int ThumbHeight;
    int Move;
    bool Drag;
    int ThumbRange;
    int ThumbOffset;
    int ThumbPos;
    int PrevThumbPos;
    double Step;


public:
    Slider(wxPanel* parent, int width, int height);
    void SetValue(int value);
    int GetValue();
    void SetRange(int minValue, int maxValue);
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    void render(wxDC&  dc);

    // some useful events
    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
};


#endif /* SLIDER_H_ */
