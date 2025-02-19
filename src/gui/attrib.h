/**
 * @file attrib.h
 * @brief imain-window object attributes header file
 * @date 01-29-2025
 */
#ifndef ATTRIB_H
#define ATTRIB_H


#include <gtk/gtk.h> // needed for PangoWeight enum and guint

/// @brief Color structure used throughout the program for UI elements
/// @par I can barely see in more than 16-bit color, so this is already way
/// more color than I can even differentiate.
typedef struct _Color16_t
{
  guint16 red;
  guint16 green;
  guint16 blue;
}Color16_t;

/// @brief Font attribute structure
typedef struct _Font_Attrib_t
{
  const char* font_family; /// $ fc-list [to get a list of font names on system]
  PangoWeight weight;      /// normal, bold, etc.
  int size;
  bool update_font_attrib = false;
}Font_Attrib_t;


/* Pango.Weight enum listing for ref:
PANGO_WEIGHT_THIN 
The thin weight (= 100) Since: 1.24
PANGO_WEIGHT_ULTRALIGHT 
The ultralight weight (= 200)
PANGO_WEIGHT_LIGHT  
The light weight (= 300)
PANGO_WEIGHT_SEMILIGHT  
The semilight weight (= 350) Since: 1.36.7
PANGO_WEIGHT_BOOK 
The book weight (= 380) Since: 1.24)
PANGO_WEIGHT_NORMAL 
The default weight (= 400)
PANGO_WEIGHT_MEDIUM 
The medium weight (= 500) Since: 1.24
PANGO_WEIGHT_SEMIBOLD 
The semibold weight (= 600)
PANGO_WEIGHT_BOLD 
The bold weight (= 700)
PANGO_WEIGHT_ULTRABOLD  
The ultrabold weight (= 800)
PANGO_WEIGHT_HEAVY  
The heavy weight (= 900)
PANGO_WEIGHT_ULTRAHEAVY 
The ultraheavy weight (= 1000) Since: 1.24
*/
#endif // ATTRIB_H
