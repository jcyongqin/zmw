/*
  ZMW: A Zero Memory Widget Library
  Copyright (C) 2002-2003 Thierry EXCOFFIER, Universit� Claude Bernard, LIRIS

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

#include <gdk/gdkkeysyms.h>
#include "zmw/zmw.h"
#include "zmw/selection.h"

void zmw_text_position(const char *text, int *x, int *y, int *height)
{
  gint lbearing, rbearing, width, ascent, descent ;

  gdk_string_extents(ZMW_FONT, text
		     , &lbearing, &rbearing, &width, &ascent, &descent
		     ) ;

  *x = ZMW_SIZE_ALLOCATED.x ; 
  *y = ZMW_SIZE_ALLOCATED.y + ZMW_SIZE_ALLOCATED.height - descent ;
  *height = ascent + descent ;
}

int zmw_text_find_cursor(const char *text)
{
  int x, y, i ;
  gint lbearing, rbearing, width, ascent, descent, height ;
	
  zmw_text_position(text, &x, &y, &height) ;
      		
  for(i=0; text[i]; i++)
    {
      gdk_text_extents(ZMW_FONT, text, i
		       , &lbearing, &rbearing, &width, &ascent, &descent
		       ) ;
      if ( zmw.x < x + width )
	break ;
    }
  return( i ) ;
}

void zmw_text_insert(char **text, int *cursor_pos, int *start_pos
		     , const char *insert)
{
  int len ;
	
  len = strlen(insert) ;
	
  if ( start_pos && *start_pos != -1
       && cursor_pos && *start_pos > *cursor_pos )
    *start_pos += len ;

  ZMW_REALLOC(*text, strlen(*text)+ len + 1) ;
  if ( cursor_pos )
    {
      memmove(*text + *cursor_pos + len
	      ,*text + *cursor_pos
	      ,strlen(*text) - *cursor_pos + 1
	      ) ;
      strncpy(*text + *cursor_pos, insert, len) ;
      *cursor_pos += len ;
    }
  else
    strcat(*text, insert) ;
}

void zmw_text_backspace(char **text, int *cursor_pos, int *start_pos)
{
  if ( *text )
    {
      if ( start_pos && *start_pos != -1
	   && cursor_pos && *start_pos > *cursor_pos )
	(*start_pos)-- ;

      if ( cursor_pos == NULL )
	(*text)[strlen(*text)-1] = '\0' ;
      else
	{
	  if ( *cursor_pos > 0 )
	    {
	      memmove(*text + *cursor_pos - 1
		      ,*text + *cursor_pos
		      ,strlen(*text) - *cursor_pos + 1
		      ) ;
	      (*cursor_pos)-- ;
	    }
	}
    }
}

void zmw_text_draw_selection(const char *text, int start_pos, int cursor_pos
			     , int x, int y, gint height
			     )
{
  gint lbearing, rbearing, ascent, descent ;
  int start, end ;

  if ( start_pos != -1 )
    {
      gdk_text_extents(ZMW_FONT, text, start_pos
		       , &lbearing, &rbearing, &start, &ascent, &descent
		       ) ;
      gdk_text_extents(ZMW_FONT, text, cursor_pos
		       , &lbearing, &rbearing, &end, &ascent, &descent
		       ) ;
      
      gdk_draw_rectangle(ZMW_WINDOW, ZMW_GC[ZMW_BACKGROUND_POPED], 1
			 , x + ZMW_MIN(start, end)
			 , ZMW_SIZE_ALLOCATED.y
			 , ZMW_ABS(start - end)
			 , ZMW_SIZE_ALLOCATED.height
			 ) ;    
    }
}

void zmw_text_draw_text(const char *text, int x, int y, Zmw_Boolean activable
			, Zmw_Boolean editable)
{
  /*
   * Draw text normal or gray if not sensitive
   */
  if ( (zMw[-1].u.size.sensible && ZMW_SENSIBLE) ||
       (!activable && !editable) )
    gdk_draw_string(ZMW_WINDOW, ZMW_FONT, ZMW_GC[ZMW_FOREGROUND]
		    , x, y, text) ;
  
  else
    {
      gdk_draw_string(ZMW_WINDOW, ZMW_FONT, ZMW_GC[ZMW_BORDER_LIGHT]
		      , x-1, y-1, text) ;
      gdk_draw_string(ZMW_WINDOW, ZMW_FONT, ZMW_GC[ZMW_BORDER_DARK]
		      , x, y, text) ;
    }
}

void zmw_text_draw_cursor(const char *text, int x, int cursor_pos)
{
  gint lbearing, rbearing, ascent, descent, width ;
  int xx ;

  gdk_text_extents(ZMW_FONT, text, cursor_pos
		   , &lbearing, &rbearing, &width, &ascent, &descent
		   ) ;
  xx = x + width ;
  if ( width != 0 )
    xx-- ;
  gdk_draw_line(ZMW_WINDOW, ZMW_GC[ZMW_BORDER_LIGHT]
			, xx
			, ZMW_SIZE_ALLOCATED.y - 1
			, xx
			, ZMW_SIZE_ALLOCATED.y + ZMW_SIZE_ALLOCATED.height
			) ;
}

void zmw_text_selection_take(const char *text, Zmw_Boolean *button_1_pressed
			     , int *start_pos, const int *cursor_pos)
{
  char *t ;
  int len ;
      		
  if ( *start_pos == *cursor_pos )
    *start_pos = -1 ;
  else
    {
      zmw_selection_take() ;
      zmw_need_repaint() ;
      len = ZMW_ABS(*start_pos - *cursor_pos) ;
      ZMW_MALLOC(t, len + 1) ;
      strncpy(t, &text[ZMW_MIN(*start_pos, *cursor_pos)], len);
      t[len] = '\0' ;
      
      zmw_set_selection("PRIMARY", "STRING", ZMW_WINDOW, t) ;
    }
  *button_1_pressed = Zmw_False ;
}

void zmw_text_cursor_set(const char *text, int *cursor_pos
			 , int *start_pos, Zmw_Boolean *button_1_pressed)
{
  int i ;

  i = zmw_text_find_cursor(text) ;
  *cursor_pos = i ;
  if ( start_pos )
    {
      *start_pos = i ;
      *button_1_pressed = Zmw_True ;
    }
}

void zmw_text_size(const char *text)
{
  gint lbearing, rbearing, width, ascent, descent ;

  gdk_string_extents(ZMW_FONT, text
		     , &lbearing, &rbearing, &width, &ascent, &descent
		     ) ;
  ZMW_SIZE_MIN.width = width ;
  ZMW_SIZE_MIN.height = ascent + descent ;
}



void zmw_text_simple(char **text, Zmw_Boolean editable, Zmw_Boolean activable
		     , int *cursor_pos, int *start_pos)
{
  int x, y ;
  static Zmw_Boolean button_1_pressed = 0 ;
  gint height ;
  int len = strlen(*text) ;
  Zmw_Resource cursor = { "TextCursorPos", len } ;
  Zmw_Resource start = { "TextStartPos", -1 } ;

  ZMW_ASSERT( *text != NULL ) ;

  if ( zmw_debug_message() )
    {
      http_printf("(%s)<BR>\n", *text) ;
    }

  if ( ( (ZMW_SUBACTION == Zmw_Input_Event
	|| ZMW_SUBACTION
	== Zmw_Compute_Children_Allocated_Size_And_Pre_Drawing)
	 ) && editable )
    {
      zmw_resource_int_get(&cursor_pos, &cursor) ;
      ZMW_CLAMP(*cursor_pos, 0, len) ;
      zmw_resource_int_get(&start_pos, &start) ;
      ZMW_CLAMP(*start_pos, -1, len) ;
    }

  switch( ZMW_SUBACTION )
    {
    case Zmw_Compute_Required_Size:
      zmw_text_size(*text) ;
      break ;

    case Zmw_Compute_Children_Allocated_Size_And_Pre_Drawing:

      if ( start_pos && !zmw_selection_have() && !button_1_pressed )
      	{
	  *start_pos = -1 ;
      	}
      zmw_text_position(*text, &x, &y, &height) ;
      if ( editable )
	zmw_text_draw_selection(*text, *start_pos, *cursor_pos, x, y, height);
      zmw_text_draw_text(*text, x, y, activable, editable) ;
      if ( cursor_pos && zmw_focused_by_its_parents() )
	zmw_text_draw_cursor(*text, x, *cursor_pos) ;
      break ;
    case Zmw_Input_Event:

      if ( !editable )
	break ;
      // if ( editable && zmw.event->type == GDK_KEY_PRESS ) zmw_event_dump() ;

      /*
       * Set the selection on button 1 release
       */
      if ( zmw_button_released_anywhere() && button_1_pressed )
	zmw_text_selection_take(*text, &button_1_pressed
				, start_pos, cursor_pos) ;
      /*
       * Position the text cursor on button 1 click
       * Or paste on button 2 click
       */
      if ( zmw_button_pressed() && cursor_pos && *text )
      	{
	  if ( zmw.event->button.button == 1 )
	    zmw_text_cursor_set(*text,cursor_pos, start_pos,&button_1_pressed);
	  else if ( zmw.event->button.button == 2 )
	    {
	      zmw_text_insert(text, cursor_pos, start_pos
			      , zmw_get_selection("PRIMARY", "STRING"
						  , ZMW_WINDOW)
			      ) ;
	      zmw_event_remove() ;
	    }
	  //	  zmw_event_remove() ;  decorator must take this event
      	}
      /*
       * Insert some key
       */
      if ( zmw_key_pressed() )
	{
	  if ( zmw.event->key.keyval == GDK_BackSpace )
	    {
	      zmw_text_backspace(text, cursor_pos, start_pos) ;
	      zmw.activated = Zmw_True ;
	    }
	  else if ( cursor_pos && zmw.event->key.keyval == GDK_Left )
	    {
	      if ( *cursor_pos > 0 )
		(*cursor_pos)-- ;
	    }
	  else if ( cursor_pos && zmw.event->key.keyval == GDK_Right )
	    {
	      if ( *text && *cursor_pos < strlen(*text) )
		(*cursor_pos)++ ;
	    }
	  else if ( zmw.event->key.string[0] )
	    {
	      zmw_text_insert(text, cursor_pos, start_pos
			      , zmw.event->key.string) ;
	      zmw.activated = Zmw_True ;
	    }  
	  zmw_event_remove() ;
	}

      if ( button_1_pressed )
      	{
	  *start_pos = zmw_text_find_cursor(*text) ;
	  if ( *start_pos != start.old_value )
	    zmw_need_repaint() ;
	}

      /* An editable text is not draggable because
       * the decorator does not receive the button press
       */
      if ( button_1_pressed && zmw.event->type == GDK_MOTION_NOTIFY
	   && editable )
	{
	  /* If the event is not removed, we see selection move
	   * on all the text widget.
	   */
	  zmw_event_remove() ;
      	}
      break ;
    default:
      break ;
    }

  zmw_resource_set(&cursor) ;
  zmw_resource_set(&start) ;
}

void zmw_text(const char *text)
{
  ZMW(zmw_text_simple( (char**)&text, Zmw_False, Zmw_False, NULL, NULL)) { }
}

void zmw_text_editable_with_cursor_and_start(char **text, int *cursor_pos
					     , int *start_pos)
{
  ZMW(zmw_decorator(  Zmw_Decorator_Border_Relief
		    | Zmw_Decorator_Border_In
		    | Zmw_Decorator_Interior
		    | Zmw_Decorator_Focusable
		      // | Zmw_Decorator_Activable
		    )
      )
    {
      ZMW(zmw_text_simple( text, Zmw_True, Zmw_False, cursor_pos, start_pos))
	{ }
      if ( zmw_activated() )
	{
	}
    }
  if ( zmw_activated() )
    {
    }

  if ( zmw.child_activated )
    {
      zmw.activated = Zmw_True ;
    }
}

void zmw_text_editable_with_cursor(char **text, int *cursor_pos)
{
  zmw_text_editable_with_cursor_and_start(text, cursor_pos, NULL) ;
}

void zmw_text_editable(char **text)
{
  zmw_text_editable_with_cursor(text, NULL) ;
}

void zmw_int_editable(int *i)
{
  char buf[20], *text ;
  
  sprintf(buf, "%d", *i) ;
  text = strdup(buf) ;
  zmw_text_editable(&text) ;
  *i = atoi(text) ;
  free(text) ;
}

void zmw_int(int i)
{
  char buf[20] ;
  
  sprintf(buf, "%d", i) ;
  zmw_text(buf) ;
}
