/*
    ZMW: A Zero Memory Widget Library
    Copyright (C) 2002-2003  Thierry EXCOFFIER, LIRIS

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: Thierry.EXCOFFIER@liris.univ-lyon1.fr
*/


void test_scrollbar(const char *title)
{
  static float v = 0.5 ;
  static GdkWindow *w = NULL ;
  char buf[99] ;

  ZMW(zmw_window(&w, title))
    {
      ZMW(zmw_box_vertical())
	{
	  sprintf(buf, "Scrollbar position = %g  ", v) ;
	  zmw_text(buf) ;
	  zmw_horizontal_expand(1) ;
	  zmw_scrollbar_horizontal(&v) ;
	}
    }
}