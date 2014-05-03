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
 * ARE DISCLAIMED. IN NO EVENT SHALL G.GIMENEZ BE LIABLE FOR ANY DIRECT,
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
