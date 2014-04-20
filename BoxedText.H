#ifndef __RAII_BOXEDTEXT_H__
#define __RAII_BOXEDTEXT_H__

#include "Cairo.H"
namespace raii {
namespace Cairo {



        class Fragment {
                public:
                String word;    //mot à afficher
                Font font;      //police du fragment
                double size;    //taille de la police
                bool underline; //souligné
                bool strike;    //barré
                Color color;    //couleur du texte
                Color bgcolor;  //couleur du fond
                double dx,dy;   //position dans la fenêtre d'affichage
                Geometry geo;   //geometrie de la chaîne
                Fragment(Document&        doc,
                         const String&   word_,
                         const Font&     font_,
                         double          size_,
                         bool       underline_,
                         bool          strike_,
                         const Color&   color_,
                         const Color& bgcolor_);
                double getWidth();
                double getAscender();
                double getDescender();

                virtual ~Fragment();

                virtual void apply(Document& doc,double x, double y);
        };


class BoxedText {

        public:


        typedef enum { left, right, center, justify, fulljustify } TextAlign;

        Document& doc;
        double origin_x,origin_y;
        double width;
        double height;
        double curHeight;
        Vector<Fragment> fragments;
        Vector<Fragment> remainder;
        Font defaultFont;
        double defaultSize;
        Color defaultColor;
        Color defaultBGColor;
        TextAlign align;

        double minAscent;
        double minDescent;
        double firstLineMargin;
//        double minWordSpacing;

        BoxedText(Document& d,
                  double x, double y,
                  double w, double h=1000000,
                  Font font=Font("Serif"), double s=12,
                  Color color=Color::Gray(0),
                  Color bgcolor=Color::RGBA(1,1,1,-1) );
        virtual ~BoxedText();

        Vector<Fragment> getRemainder();
        void setFragments(Vector<Fragment> f);
        double getHeight();
        void setTextAlign(TextAlign a);

        void addFragment(const String&   word,
                         Font     font=Font("plop"),
                         double          size=-1,
                         bool       underline=false,
                         bool          strike=false,
                         Color   color=Color::RGBA(0,0,0,-1),
                         Color bgcolor=Color::RGBA(1,1,1,-1));

        void translate(Vector<Fragment>::iterator begin, Vector<Fragment>::iterator& end, double dx, double dy);
        void setdY(Vector<Fragment>::iterator begin, Vector<Fragment>::iterator& end, double dy);
        int count(Vector<Fragment>::iterator begin, Vector<Fragment>::iterator& end);
        Geometry getGeometry(Vector<Fragment>::iterator begin, Vector<Fragment>::iterator& end);
        void compute();
        void apply();

};

#undef min
#undef max

}
}
#endif /* __RAII_BOXEDTEXT_H__ */
