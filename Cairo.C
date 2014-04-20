#include "raii.H"
#include "Cairo.H"
#include <cairo-pdf.h>
#include <cairo-ps.h>
#include <cairo-svg.h>
#include <pango/pangocairo.h>
//#include <cpta/Cairo.H>

namespace raii {
	namespace Cairo {

		extern "C" {//int raii_cairo_write (void *closure, unsigned char *data, unsigned int length);

			cairo_status_t raii_cairo_write(void *closure, const unsigned char *data,
					unsigned int length) {
				//Cairo::Cairo *doc = reinterpret_cast<Cairo::Cairo*>(closure);
				if (!raii::apacheRequest)
					return cairo_status_t(11);

				if (ap_rwrite(data, length, raii::apacheRequest) < 0)
					return cairo_status_t(11);
				return cairo_status_t(0);

			}
		}

		void cairo_check(cairo_t *cr) {
			switch (cairo_status(cr)) {
				case CAIRO_STATUS_SUCCESS:
					return;
				default:
					throw CairoException(cairo_status_to_string(cairo_status(cr)));
			}

		}

		Font::Font() {
			undef = true;
		}

		Font::Font(const String& f, Slant s, Weight w) :
					family(f), slant(s), weight(w), undef(false) {
		}

		String Font::getDescriptionString(double size) const {
			if (undef)
				throw CairoException("Undefined font!");
			String ret = family;
			switch (slant) {
				case regular:
				default:
					break;
				case italic:
					ret += " italic";
					break;
				case oblique:
					ret += " oblique";
					break;
			}
			switch (weight) {
				case normal:
				default:
					break;
				case bold:
					ret += " bold";
					break;
			}
			ret = ret + " " + ftostring(size);
			return ret;
		}

		Color::Color() {
			undef = true;
		}

		Color::Color(double r, double g, double b, double a) :
					red(r), green(g), blue(b), alpha(a), undef(false) {
		}

		String Color::getType() {
			return "fillstroke";
		}

		String Color::getColorSpace() {
			return "rgb";
		}

		Color Color::Gray(double value) {
			return Color(value, value, value);
		}
		Color Color::RGB(double r, double g, double b) {
			return Color(r, g, b);
		}
		Color Color::RGBA(double r, double g, double b, double a) {
			return Color(r, g, b, a);
		}
		Color Color::CMYK(double c, double m, double y, double k) {
			throw Exception("CMYK NI");
			return Color(0, 0, 0);
		}
		void Color::select(void *cr) const {
			cairo_t *c = static_cast<cairo_t*> (cr);
			if (undef)
				throw CairoException("Undefined color");
			cairo_set_source_rgba(c, red, green, blue, alpha);
			cairo_check(c);
		}

		Geometry::Geometry(double w, double d, double a, double h) {
			dimension[width] = w;
			dimension[ascender] = a;
			dimension[descender] = d;
			height = h;
		}

		double Geometry::getWidth() {
			return dimension[width];
		}
		double Geometry::getAscender() {
			return dimension[ascender];
		}
		double Geometry::getDescender() {
			return dimension[descender];
		}
		double Geometry::getHeight() {
			return height;
		}

		//objet cairo/pango en ref:
		// surface, cr, layout, fontmap, pangoctx
		class Cairo: public Object {
			public:
			String filename;
			double width, height;
			cairo_surface_t *surface;
			cairo_t *cr;
			PangoLayout *layout;
			PangoFontMap *fontmap;
			PangoContext *pangoctx;
			SurfaceType surfaceType;
			Font currentFont;
			double currentSize;

			Cairo(const String& fname, double w, double h, SurfaceType t) :
				filename(fname), width(w), height(h), surface(NULL), cr(NULL), layout(
						NULL), fontmap(NULL), pangoctx(NULL), surfaceType(t) {

				if (!filename.empty()) {
					switch (t) {
						case PDF:
						default:
							surface = cairo_pdf_surface_create(filename.c_str(), width,
									height);
							break;
						case PS:
							surface = cairo_ps_surface_create(filename.c_str(), width,
									height);
							break;
						case SVG:
							surface = cairo_svg_surface_create(filename.c_str(), width,
									height);
							break;
						case RGB:
							surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
									(int) width, (int) height);
							break;
						case ARGB:
							surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
									(int) width, (int) height);
							break;
					}
				}
				else {
					switch (t) {
						case PDF:
						default:
							surface = cairo_pdf_surface_create_for_stream(raii_cairo_write,
									NULL, width, height);
							break;
						case PS:
							surface = cairo_ps_surface_create_for_stream(raii_cairo_write,
									NULL, width, height);
							break;
						case SVG:
							surface = cairo_svg_surface_create_for_stream(raii_cairo_write,
									NULL, width, height);
							break;
						case RGB:
							surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
									(int) width, (int) height);
							break;
						case ARGB:
							surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
									(int) width, (int) height);
							break;
					}
				}
				if (!surface)
					throw CairoException("Unable to create Cairo Surface");
				cr = cairo_create(surface);
				if (!cr)
					throw CairoException("Unable to create Cairo Context");
				cairo_check(cr);

				layout = pango_cairo_create_layout(cr);
				if (!layout)
					throw CairoException("Unable to create Pango Layout");

				fontmap = PANGO_FONT_MAP(pango_cairo_font_map_get_default());
				if (!fontmap)
					throw CairoException("Unable to get default fontmap");

				pangoctx = pango_font_map_create_context(fontmap);
				if (!pangoctx)
					throw CairoException("Unable to get Pango Context");

			}

			virtual ~Cairo() {
				if (pangoctx)
					g_object_unref(pangoctx);
				if (layout)
					g_object_unref(layout);
				if (surface)
					cairo_surface_flush(surface);
				if (cr)
					cairo_destroy(cr);
				if (surface)
					cairo_surface_destroy(surface);
			}

			void setCurrentFont(const Font& f) {
				currentFont = f;
			}
			Font getCurrentFont() {
				return currentFont;
			}
			void setCurrentSize(double s) {
				currentSize = s;
			}
			double getCurrentSize() {
				return currentSize;
			}
		};

		//pas d'objet cairo/pango en ref

		namespace Operation {

			class CheckPoint: public Generic {
				public:
				CheckPoint() :
					Generic() {
					checkpoint = true;
				}
				virtual void apply(ptr<Cairo> cairo) {
					/*noop*/
					throw OperationException("un checkpoint n'est pas exécutable");
				}
			};

			class Line: public Generic {
				double x1, y1, x2, y2;
				public:
				Line(double x1_, double y1_, double x2_, double y2_) :
					Generic(), x1(x1_), y1(y1_), x2(x2_), y2(y2_) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					double cx = 0., cy = 0.;
					cairo_get_current_point(cairo->cr, &cx, &cy);
					cairo_check(cairo->cr);
					cairo_move_to(cairo->cr, x1, y1);
					cairo_check(cairo->cr);
					cairo_line_to(cairo->cr, x2, y2);
					cairo_check(cairo->cr);
					cairo_stroke(cairo->cr);
					cairo_check(cairo->cr);
					cairo_move_to(cairo->cr, cx, cy);
					cairo_check(cairo->cr);
				}
			};
			class SetLineWidth: public Generic {
				double width;
				public:
				SetLineWidth(double w) :
					Generic(), width(w) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_set_line_width(cairo->cr, width);
					cairo_check(cairo->cr);
				}
			};
			class Rectangle: public Generic {
				double x1, y1, x2, y2;
				public:
				Rectangle(double x1_, double y1_, double x2_, double y2_) :
					Generic(), x1(x1_), y1(y1_), x2(x2_), y2(y2_) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					double cx = 0., cy = 0.;
					cairo_get_current_point(cairo->cr, &cx, &cy);
					cairo_check(cairo->cr);
					cairo_rectangle(cairo->cr, x1, y1, x2 - x1, y2 - y1);
					cairo_check(cairo->cr);
					cairo_stroke(cairo->cr);
					cairo_check(cairo->cr);
					cairo_move_to(cairo->cr, cx, cy);
					cairo_check(cairo->cr);
				}
			};
			class FilledRectangle: public Generic {
				double x1, y1, x2, y2;
				public:
				FilledRectangle(double x1_, double y1_, double x2_, double y2_) :
					Generic(), x1(x1_), y1(y1_), x2(x2_), y2(y2_) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					double cx = 0., cy = 0.;
					cairo_get_current_point(cairo->cr, &cx, &cy);
					cairo_check(cairo->cr);
					cairo_rectangle(cairo->cr, x1, y1, x2 - x1, y2 - y1);
					cairo_check(cairo->cr);
					cairo_fill(cairo->cr);
					cairo_check(cairo->cr);
					cairo_move_to(cairo->cr, cx, cy);
					cairo_check(cairo->cr);
				}
			};
			class SelectFont: public Generic {
				Font font;
				double size;
				public:
				SelectFont(const Font& f, double s) :
					Generic(), font(f), size(s) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					PangoFontDescription *desc = pango_font_description_from_string(
							font.getDescriptionString(size).c_str());
					pango_layout_set_font_description(cairo->layout, desc);
					pango_font_description_free(desc);
					cairo_check(cairo->cr);
					cairo->setCurrentFont(font);
					cairo->setCurrentSize(size);
				}
			};
			class EndPage: public Generic {
				public:
				EndPage() :
					Generic() {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_show_page(cairo->cr);
					cairo_check(cairo->cr);
				}
			};
			class AddText: public Generic {
				String str;
				bool alignOnTextBase;
				public:
				AddText(const String& s, bool va = true) :
					Generic(), str(s), alignOnTextBase(va) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_save(cairo->cr);
					cairo_check(cairo->cr);

					if (alignOnTextBase) {
						PangoFontDescription *desc = pango_font_description_from_string(
								cairo->getCurrentFont().getDescriptionString(
										cairo->getCurrentSize()).c_str());
						if (!desc)
							throw NullPointerException("desc is null");

						pango_layout_set_font_description(cairo->layout, desc);
						pango_layout_set_text(cairo->layout, str.c_str(), -1);

						PangoFont *pf = pango_font_map_load_font(PANGO_FONT_MAP(
								cairo->fontmap), cairo->pangoctx, desc);
						pango_font_description_free(desc);

						if (!pf)
							throw NullPointerException("pf is null");
						PangoLanguage *language = pango_language_from_string("fr_FR.UTF-8");
						if (!language)
							throw NullPointerException("language is null");
						PangoFontMetrics * metrics = pango_font_get_metrics(pf, language);
						if (!metrics)
							throw NullPointerException("metrics is null");

						cairo_rel_move_to(cairo->cr, 0, -pango_font_metrics_get_ascent(
								metrics) / PANGO_SCALE);
						pango_font_metrics_unref(metrics);
					}
					else {
						pango_layout_set_text(cairo->layout, str.c_str(), -1);

					}
					pango_cairo_show_layout(cairo->cr, cairo->layout);
					cairo_check(cairo->cr);
					cairo_restore(cairo->cr);
					cairo_check(cairo->cr);
				}
			};
			class SetColor: public Generic {
				Color color;
				public:
				SetColor(const Color& c) :
					Generic(), color(c) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					color.select(cairo->cr);
				}
			};
			class GotoXY: public Generic {
				double x;
				double y;
				public:
				GotoXY(double x_, double y_) :
					Generic(), x(x_), y(y_) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_move_to(cairo->cr, x, y);
					cairo_check(cairo->cr);
				}
			};

			//opérations graphiques
			class MoveTo: public Generic {
				double x;
				double y;
				public:
				MoveTo(double x_, double y_) :
					Generic(), x(x_), y(y_) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_move_to(cairo->cr, x, y);
					cairo_check(cairo->cr);
				}
			};
			class RelMoveTo: public Generic {
				double dx;
				double dy;
				public:
				RelMoveTo(double dx_, double dy_) :
					Generic(), dx(dx_), dy(dy_) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_rel_move_to(cairo->cr, dx, dy);
					cairo_check(cairo->cr);
				}
			};
			class LineTo: public Generic {
				double x;
				double y;
				public:
				LineTo(double x_, double y_) :
					Generic(), x(x_), y(y_) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_line_to(cairo->cr, x, y);
					cairo_check(cairo->cr);
				}
			};
			class RelLineTo: public Generic {
				double dx;
				double dy;
				public:
				RelLineTo(double dx_, double dy_) :
					Generic(), dx(dx_), dy(dy_) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_rel_line_to(cairo->cr, dx, dy);
					cairo_check(cairo->cr);
				}
			};
			class CurveTo: public Generic {
				double x1, y1, x2, y2, x3, y3;
				public:
				CurveTo(double x1_, double y1_, double x2_, double y2_, double x3_,
						double y3_) :
							Generic(), x1(x1_), y1(y1_), x2(x2_), y2(y2_), x3(x3_), y3(y3_) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_curve_to(cairo->cr, x1, y1, x2, y2, x3, y3);
					cairo_check(cairo->cr);
				}
			};
			class RelCurveTo: public Generic {
				double dx1, dy1, dx2, dy2, dx3, dy3;
				public:
				RelCurveTo(double dx1_, double dy1_, double dx2_, double dy2_, double dx3_,
						double dy3_) :
							Generic(), dx1(dx1_), dy1(dy1_), dx2(dx2_), dy2(dy2_), dx3(dx3_), dy3(
									dy3_) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_rel_curve_to(cairo->cr, dx1, dy1, dx2, dy2, dx3, dy3);
					cairo_check(cairo->cr);
				}
			};
			class Arc: public Generic {
				double xc, yc, radius, angle1, angle2;
				public:
				Arc(double x, double y, double r, double a1, double a2) :
					Generic(), xc(x), yc(y), radius(r), angle1(a1), angle2(a2) {
				}
				virtual void apply(ptr<Cairo> cairo) {
					// commence dans l'angle des X positifs puis continue vers les y positifs
					// donc vers le bas
					cairo_arc(cairo->cr, xc, yc, radius, angle1, angle2);
				}
			};
			class ClearPath: public Generic {
				public:
				ClearPath() :
					Generic() {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_new_path(cairo->cr);
				}
			};
			class ClosePath: public Generic {
				public:
				ClosePath() :
					Generic() {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_close_path(cairo->cr);
				}
			};
			class Fill: public Generic {
				public:
				Fill() :
					Generic() {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_fill(cairo->cr);
				}
			};
			class Stroke: public Generic {
				public:
				Stroke() :
					Generic() {
				}
				virtual void apply(ptr<Cairo> cairo) {
					cairo_stroke(cairo->cr);
				}
			};
		}

		Document::Document(const String& fname, double width, double height,
				SurfaceType t) :
				doc(NULL), saved(false), debugEnabled(false), color(), font(), x(0), y(0),
				margin(0) {
			doc = new Cairo(fname, width, height, t);
		}

		Document::Document(const String& fname, PageSize ps, Orientation o,
				SurfaceType t) :
				doc(NULL), saved(false), debugEnabled(false), color(), font(), x(0), y(0),
				margin(0) {

			double width, height;
			switch (ps) {
				case A5:
					width = 419.53;
					height = 595.28;
					break;
				case A4:
				default:
					width = 595.28;
					height = 841.89;
					break;
				case A3:
					width = 841.89;
					height = 1190.55;
					break;
			}
			if (o == PORTRAIT)
				doc = new Cairo(fname, width, height, t);
			else
				doc = new Cairo(fname, height, width, t);
		}

		Document::~Document() {
			if (!saved)
				try {
					save();
				} catch (...) {
				}
				switch (doc->surfaceType) {
					case RGB:
					case ARGB:

						writeToPNG(doc->filename);
						break;
					default:
						//noop
						break;
				}
				doc = NULL;
		}
		void Document::setDebug(bool b) {
			debugEnabled = b;
		}
		void Document::debug(const String& message) {
			if (debugEnabled) {
				Logger log("Cairo::Document");
				log(message);
			}
		}

		void Document::writeToPNG(const String& pngFile) {
			debug("writeToPNG");
			if (!pngFile.empty())
				cairo_surface_write_to_png(doc->surface, pngFile.c_str());
			else
				cairo_surface_write_to_png_stream(doc->surface, raii_cairo_write, NULL);
		}

		void Document::save() {
			debug("save");
			saved = true;
			debug("applying op begin");

			for (Vector<ptr<Operation::Generic> >::iterator op = operations.begin(); op
			!= operations.end(); ++op) {

				if ((*op)->checkpoint) {
					debug("checkpoint: break!");
					break;
				}
				(*op)->apply(doc);

			}
			debug("applying op end");
		}

		Font Document::loadFont(const String& fontname, Font::Slant s, Font::Weight w) {
			debug("loadFont");
			return Font(fontname, s, w);
		}

		Geometry Document::getStringGeometry(const String& text, const Font& font,
				double size) {

			debug("getStringGeometry");
			cairo_save(doc->cr);
			cairo_check(doc->cr);

			PangoFontDescription *desc = pango_font_description_from_string(
					font.getDescriptionString(size).c_str());
			if (!desc)
				throw NullPointerException("desc is null");
			pango_layout_set_font_description(doc->layout, desc);
			pango_layout_set_text(doc->layout, text.c_str(), -1);

			PangoFont *pf = pango_font_map_load_font(PANGO_FONT_MAP(doc->fontmap),
					doc->pangoctx, desc);
			pango_font_description_free(desc);
			if (!pf)
				throw NullPointerException("pf is null");
			PangoLanguage *language = pango_language_from_string("fr_FR.UTF-8");
			if (!language)
				throw NullPointerException("language is null");
			PangoRectangle logical_rect;
			pango_layout_get_extents(doc->layout, NULL, &logical_rect);

			cairo_restore(doc->cr);
			cairo_check(doc->cr);

			PangoFontMetrics * metrics = pango_font_get_metrics(pf, language);
			if (!metrics)
				throw NullPointerException("metrics is null");

			Geometry geo(logical_rect.width / PANGO_SCALE,
					pango_font_metrics_get_descent(metrics) / PANGO_SCALE,
					-pango_font_metrics_get_ascent(metrics) / PANGO_SCALE,
					logical_rect.height / PANGO_SCALE);

			pango_font_metrics_unref(metrics);
			return geo;
		}

		double Document::getFontHeight(const Font& font, double size) {

			debug("getFontHeight");
			cairo_save(doc->cr);
			cairo_check(doc->cr);

			PangoFontDescription *desc = pango_font_description_from_string(
					font.getDescriptionString(size).c_str());
			PangoFont *pf = pango_font_map_load_font(PANGO_FONT_MAP(doc->fontmap),
					doc->pangoctx, desc);
			pango_font_description_free(desc);
			PangoLanguage *language = pango_language_from_string("fr_FR.UTF-8");
			PangoFontMetrics * metrics = pango_font_get_metrics(pf, language);

			double height = (pango_font_metrics_get_descent(metrics)
					+ pango_font_metrics_get_ascent(metrics)) / PANGO_SCALE;
			pango_font_metrics_unref(metrics);
			cairo_restore(doc->cr);
			cairo_check(doc->cr);
			return height;
		}
		double Document::getStringWidth(const String& text, const Font& font,
				double size) {
			debug("getStringWidth");
			Geometry geo = getStringGeometry(text, font, size);
			//return geo.getWidth() ? geo.getWidth() : getStringWidth("e",font,size)*text.length();
			return geo.getWidth();
		}

		void Document::beginPage() {
			/*noop*/
		}

		void Document::addInfo(const String& key, const String& value) {
			throw Exception("addInfo() NI");
		}

		void Document::endPage() {
			debug("endPage()");
			operations.push_back(new Operation::EndPage());
		}

		void Document::addText(const String& str) {
			operations.push_back(new Operation::AddText(str, true));
		}

		void Document::addLabel(const String& str) {
			operations.push_back(new Operation::AddText(str, false));
		}

		void Document::setLineWidth(double width) {
			operations.push_back(new Operation::SetLineWidth(width));
		}

		void Document::line(double x1, double y1, double x2, double y2) {
			operations.push_back(new Operation::Line(x1, y1, x2, y2));
		}

		void Document::rectangle(double x1, double y1, double x2, double y2) {
			operations.push_back(new Operation::Rectangle(x1, y1, x2, y2));
		}

		void Document::filledRectangle(double x1, double y1, double x2, double y2) {
			operations.push_back(new Operation::FilledRectangle(x1, y1, x2, y2));
		}

		void Document::selectFont(const Font& f, double size) {
			operations.push_back(new Operation::SelectFont(f, size));
		}

		void Document::setColor(const Color& color) {
			operations.push_back(new Operation::SetColor(color));
		}

		void Document::gotoXY(double x, double y) {
			operations.push_back(new Operation::GotoXY(x, y));
		}

		void Document::transaction(const String& str) {
			debug("transaction()");
			if (str == "begin") {
				operations.push_back(new Operation::CheckPoint());
			}
			else if (str == "commit") {
				Vector<ptr<Operation::Generic> >::iterator op = operations.end();
				do {
					--op;
					if ((*op)->checkpoint)
						operations.erase(op);
				} while (op != operations.begin());
			}
			else {
				Vector<ptr<Operation::Generic> >::iterator op = operations.end();
				do {
					bool ok = false;
					--op;
					if ((*op)->checkpoint)
						ok = true;
					operations.erase(op);
					if (ok)
						return;
				} while (op != operations.begin());
			}
		}

		void Document::moveTo(double x, double y) {
			operations.push_back(new Operation::MoveTo(x, y));
		}

		void Document::relMoveTo(double dx, double dy) {
			operations.push_back(new Operation::RelMoveTo(dx, dy));
		}

		void Document::lineTo(double x, double y) {
			operations.push_back(new Operation::LineTo(x, y));
		}

		void Document::relLineTo(double dx, double dy) {
			operations.push_back(new Operation::RelLineTo(dx, dy));
		}

		void Document::curveTo(double x1, double y1, double x2, double y2, double x3,
				double y3) {
			operations.push_back(new Operation::CurveTo(x1, y1, x2, y2, x3, y3));
		}

		void Document::relCurveTo(double dx1, double dy1, double dx2, double dy2,
				double dx3, double dy3) {
			operations.push_back(new Operation::CurveTo(dx1, dy1, dx2, dy2, dx3, dy3));
		}

		void Document::arc(double xc, double yc, double radius, double angle1, double angle2) {
			operations.push_back(new Operation::Arc(xc, yc, radius, angle1, angle2));
		}

		void Document::clearPath() {
			operations.push_back(new Operation::ClearPath());
		}

		void Document::closePath() {
			operations.push_back(new Operation::ClosePath());
		}

		void Document::fill() {
			operations.push_back(new Operation::Fill());
		}

		void Document::stroke() {
			operations.push_back(new Operation::Stroke());
		}

		double Document::getWidth() {
			return doc->width;
		}
		double Document::getHeight() {
			return doc->height;
		}

		/*
 void setLineThick();
 void setLineDotted();
 void setLineThin();
 void setLineDashed();
 void ezText();

 void ezSetMargins();
 void ezOutput();
		 */

	}
}
