/* 
 * Copyright (c) 2005-2011, Guillaume Gimenez <guillaume@blackmilk.fr>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of G.Gimenez nor the names of its contributors may
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL G.Gimenez SA BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors:
 *     * Guillaume Gimenez <guillaume@blackmilk.fr>
 *
 */
#ifndef __RAII_CAIRO_H__
#define __RAII_CAIRO_H__

#include "raii.H"

namespace raii {
	namespace Cairo {

		DECLARE_EXCEPTION(CairoException,Exception);

		typedef enum { PDF, PS, SVG, RGB, ARGB } SurfaceType;

		// pas d'objet cairo/pango
		class Font : public Object {
			public:
			DECLARE_EXCEPTION(FontException,Exception);
			//avec l'espoir naïf que les enums en bougent pas
			typedef enum {
				regular /*= CAIRO_FONT_SLANT_NORMAL*/,
				italic /*= CAIRO_FONT_SLANT_ITALIC*/,
				oblique /*= CAIRO_FONT_SLANT_OBLIQUE*/
			} Slant;
			typedef enum {
				normal /*= CAIRO_FONT_WEIGHT_NORMAL*/,
				bold /*= CAIRO_FONT_WEIGHT_BOLD*/
			} Weight;
			String family;
			Slant slant;
			Weight weight;
			bool undef;
			Font();

			Font(const String& f, Slant s=regular, Weight w=normal);

			String getDescriptionString(double size) const;

		};

		//pas d'objet cairo/pango en ref
		class Color : public Object {

			public:
			double red,green,blue;
			double alpha;
			bool undef;

			private:
			Color(double r, double g, double b, double a=1.);

			public:
			Color();

			String getType();
			String getColorSpace();
			static Color Gray(double value);
			static Color RGB(double r, double g, double b);
			static Color RGBA(double r, double g, double b, double a);
			static Color CMYK(double c, double m, double y, double k);
			virtual void select( void *cr) const;
		};

		//pas d'objet cairo/pango en ref
		class Geometry : public Object {
			friend class Document;
			typedef enum {width=0,descender,ascender,tabsize} Dimension;
			double dimension[tabsize];
			double height;
			public:

			Geometry(double w=0, double d=0, double a=0, double h=0);

			double getWidth();
			double getAscender();
			double getDescender();
			double getHeight();
		};
		class Cairo;

		namespace Operation {

			DECLARE_EXCEPTION(OperationException,Exception);

			class Generic : public Object {
				public:
				bool checkpoint;
				Generic()
				:  Object(), checkpoint(false) {}
				virtual void apply(ptr<Cairo> cairo)=0;
				virtual ~Generic() {}
			};

		}

		class Document : public Object {

			Vector<ptr<Operation::Generic> > operations;
			ptr<Cairo> doc;
			bool saved;

			bool debugEnabled;

			public:


			//commodité : espace de travail
			// ces variables ne sont pas utilisées par la classe
			Map<String,Color> color;
			Map<String,Font> font;
			double x,y;
			double margin;
			//~


			typedef enum { A5,A4,A3 } PageSize;
			typedef enum { PORTRAIT, LANDSCAPE } Orientation;


			Document(const String& fname, double width, double height, SurfaceType t=PDF);
			Document(const String& fname, PageSize ps=A4, Orientation o=PORTRAIT, SurfaceType t=PDF);


			virtual ~Document();

			void setDebug(bool b);
			void debug(const String& message);

			void writeToPNG(const String& pngFile);

			void save();

			Font loadFont(const String& fontname, Font::Slant s, Font::Weight w);




			Geometry getStringGeometry(const String& text, const Font& font, double size);

			double getFontHeight(const Font& font, double size);

			double getStringWidth(const String& text, const Font& font, double size);

			void beginPage();

			void addInfo(const String& key, const String& value);

			void endPage();

			void addText(const String& str);

			void addLabel(const String& str);

			void setLineWidth(double width);

			void line(double x1, double y1, double x2, double y2);

			void rectangle(double x1, double y1, double x2, double y2);

			void filledRectangle(double x1, double y1, double x2, double y2);

			void selectFont(const Font& f, double size);

			void setColor(const Color& color);

			void gotoXY(double x, double y);

			void transaction(const String& str);

			void moveTo(double x, double y);

			void relMoveTo(double dx, double dy);

			void lineTo(double x, double y);

			void relLineTo(double dx, double dy);

			void curveTo(double x1, double y1, double x2, double y2, double x3, double y3);

			void relCurveTo(double dx1, double dy1, double dx2, double dy2, double dx3, double dy3);

			void arc(double xc, double yc, double radius, double angle1, double angle2);

			void clearPath();

			void closePath();

			void fill();
			void stroke();

			double getWidth();
			double getHeight();



			/*
	void setLineThick();
	void setLineDotted();
	void setLineThin();
        void setLineDashed();
        void ezText();

	void ezSetMargins();
	void ezOutput();
			 */
		};


	}
}
#endif /* __RAII_CAIRO_H__ */
