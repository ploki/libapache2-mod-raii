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
#include "BoxedText.H"
namespace raii {
namespace Cairo {



        Fragment::Fragment(Document&        doc,
                         const String&   word_,
                         const Font&     font_,
                         double          size_,
                         bool       underline_,
                         bool          strike_,
                         const Color&   color_,
                         const Color& bgcolor_)
                 :      word(word_),
                        font(font_),
                        size(size_),
                        underline(underline_),
                        strike(strike_),
                        color(color_),
                        bgcolor(bgcolor_),
                        dx(0), dy(0),
                        geo(0.,0.,0.) {
                                geo=doc.getStringGeometry(word,font,size);
                        }
                double Fragment::getWidth() { return geo.getWidth(); }
                double Fragment::getAscender() { return geo.getAscender(); }
                double Fragment::getDescender() { return geo.getDescender(); }

                Fragment::~Fragment() {}

                void Fragment::apply(Document& doc,double x, double y) {


                        //fond
                        if ( bgcolor.alpha  > -0.5 ) {
                                doc.setColor( bgcolor );
                                doc.filledRectangle(x+dx,y+dy+geo.getDescender(),
                                                    x+dx+geo.getWidth(),y+dy+geo.getAscender());
                        }
                        //selection de la police
                        doc.selectFont(font,size);
                        //selection de la couleur
                        if ( color.alpha > -0.5 ) {
                                doc.setColor( color );
                        }

                        //affichage du text aligné sur la base ( appel à addText )
                        doc.gotoXY(x+dx,y+dy);
                        doc.addText(word);

                        //souligné ?
                        if ( underline )
                                doc.line(x+dx                , y+dy + ( geo.getDescender()/2 ) ,
                                         x+dx+geo.getWidth() , y+dy + ( geo.getDescender()/2 ) );
                        //barré ?
                        if ( strike )
                                doc.line(x+dx                , y+dy - ( geo.getDescender()/2 ) ,
                                         x+dx+geo.getWidth() , y+dy - ( geo.getDescender()/2 ) );

                }



        BoxedText::BoxedText(Document& d,
                  double x, double y,
                  double w, double h,
                  Font font, double s,
                  Color color,
                  Color bgcolor)
                : doc(d),
                  origin_x(x), origin_y(y),
                  width(w), height(h), curHeight(0),
                  fragments(),
                  remainder(),
                  defaultFont(font),
                  defaultSize(s),
                  defaultColor(color), defaultBGColor(bgcolor),
                  align(left) ,
                  minAscent(0),minDescent(0),
                  firstLineMargin(0)/*, minWordSpacing(0)*/ {
        }

        BoxedText::~BoxedText() {}

        Vector<Fragment> BoxedText::getRemainder() {
                return remainder;
        }
        void BoxedText::setFragments(Vector<Fragment> f) {
                fragments=f;
                remainder=Vector<Fragment>();
        }
        double BoxedText::getHeight() { return curHeight; }

        void BoxedText::setTextAlign(TextAlign a) {
                align=a;
        }
        void BoxedText::addFragment(const String&   word,
                         Font     font,
                         double          size,
                         bool       underline,
                         bool          strike,
                         Color   color,
                         Color bgcolor) {
                if ( font.family == "plop" )
                        font=defaultFont;
                if ( size < -0.5 )
                        size=defaultSize;
                if ( color.alpha < -0.5 )
                        color=defaultColor;
                if ( bgcolor.alpha < -0.5 )
                        bgcolor=defaultBGColor;
                Fragment frag(doc,word,font,size,underline,strike,color,bgcolor);
                fragments.push_back(frag);
        }

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

        void BoxedText::translate(Vector<Fragment>::iterator begin, Vector<Fragment>::iterator& end,
                        double dx, double dy) {
                do {
                        begin->dx+=dx;
                        begin->dy+=dy;
                } while ( ++begin != end );
        }
        void BoxedText::setdY(Vector<Fragment>::iterator begin, Vector<Fragment>::iterator& end, double dy) {
                do {
                        begin->dy=dy;
                } while ( ++begin != end );
        }
        int BoxedText::count(Vector<Fragment>::iterator begin, Vector<Fragment>::iterator& end) {
                int i=0;
                do {
                        ++i;
                } while ( ++begin != end );
        return i;
        }
        Geometry BoxedText::getGeometry(Vector<Fragment>::iterator begin, Vector<Fragment>::iterator& end) {
                double lascent=0,ldescent=0,lwidth=0,mwidth=0;
                do {
                        lascent = min ( lascent, begin->getAscender() );
                        ldescent = max ( ldescent, begin->getDescender() );
                        lwidth = begin->dx + begin->getWidth() ;
                        mwidth = max ( mwidth, begin->getWidth() );
                        if ( mwidth > width )
                                throw CairoException("Frag larger than width");
                } while ( ++begin != end );
                return Geometry(lwidth,ldescent,lascent);
        }

        void BoxedText::compute() {

                //double wordSpace=5;

                /*
                firstLineMargin=20.;
                minAscent=-19;
                minDescent=5;
                */

                double curdx=firstLineMargin;

                //espacement des frags
                for (   Vector<Fragment>::iterator frag = fragments.begin();
                        frag != fragments.end();
                        ++frag ) {

                               double spaceWidth=doc.getStringWidth(" ",frag->font,frag->size);
                               frag->dx = curdx;
                               curdx+=frag->getWidth()+spaceWidth;
                }

                //passage à la ligne de frags
                Vector< Vector<Fragment>::iterator >     beginOfLines;
                //beginOfLines.push_back( fragments.begin() );

                curdx=firstLineMargin;
                for (   Vector<Fragment>::iterator frag = fragments.begin();
                        frag != fragments.end();
                        ++frag ) {

                               double spaceWidth=doc.getStringWidth(" ",frag->font,frag->size);

                               if (  curdx + frag->getWidth()  > width  ) {
                                //gérer le retour chariot
                                        Vector<Fragment>::iterator frag_end=fragments.end();
                                        translate(frag,frag_end,-curdx,0.);
                                        beginOfLines.push_back(frag);
                                        curdx=0;

                               }
                               frag->dx = curdx;
                               curdx+=frag->getWidth();
                               if ( curdx + spaceWidth < width )
                                curdx+=spaceWidth;
                }

                //translation des lignes
                double curdy=0;
                Vector<Fragment>::iterator last_frag = fragments.begin();
                
                int co = 0;
                for (   Vector< Vector<Fragment>::iterator >::iterator it = beginOfLines.begin();
                        true;
                        ++it ) {
                        ++co;
                        Vector<Fragment>::iterator frag;
                        if ( it == beginOfLines.end() )
                                frag=fragments.end();
                        else
                                frag = *it ;

                        Geometry geo=getGeometry(last_frag,frag);
                        Vector<Fragment>::iterator frag_end=fragments.end();

                        double newYpos= curdy + max ( -minAscent, - geo.getAscender() )
                                + max ( minDescent , geo.getDescender() );

                        if ( newYpos > height ) {
                                remainder=Vector<Fragment>(last_frag,frag_end);

                                //la ligne qui devait être affichée ne rentre pas de le cadre
                                //throw CairoException("Out of height");
                                remainder=Vector<Fragment>(last_frag,frag_end);
                                if ( it == beginOfLines.begin() ) {
                                       fragments=Vector<Fragment>();
                                       curHeight=newYpos;
                                       return;
                                }
                                --it;

                                last_frag=*it;

                                beginOfLines.erase(it,beginOfLines.end());
                                fragments.erase(last_frag,frag_end);
                                break;

                        }

                        setdY(last_frag,frag_end,
                               curdy + max ( -minAscent, - geo.getAscender() ) );

                        curdy=newYpos;

                        last_frag=frag;
                        if ( it == beginOfLines.end() ) {
                                break;
                        }

                }

                curHeight=curdy;
                //application de la justification
                last_frag = fragments.begin();
                for (   Vector< Vector<Fragment>::iterator >::iterator it = beginOfLines.begin();
                         true;
                         ++it) {


                        Vector<Fragment>::iterator frag;

                        if ( it == beginOfLines.end() ) {
                                frag = fragments.end();
                                if ( align == justify && remainder.empty() )
                                        break;
                        }
                        else
                                frag = *it ;

                        Geometry geo=getGeometry(last_frag,frag);
                        double space=width - geo.getWidth();
                        double padding_left=0;

                        switch (align) {
                                case left: default:
                                        //noop
                                        break;
                                case right:
                                        padding_left=space;
                                        translate(last_frag,frag,padding_left,0);
                                        break;
                                case center:
                                        padding_left=space/2;
                                        translate(last_frag,frag,padding_left,0);
                                        break;
                                case fulljustify:
                                case justify: {
                                        int c=count(last_frag,frag),cc=0;
                                        space=space/(c-1);
                                        for ( Vector<Fragment>::iterator j=last_frag;
                                              j != frag;
                                              ++j ) {
                                                j->dx+=space*(cc++);
                                        }
                                        }

                        }
                        last_frag=frag;
                        if ( it == beginOfLines.end() ) {
                                break;
                        }
                }


        }


        void BoxedText::apply() {

                for (   Vector<Fragment>::iterator frag = fragments.begin();
                        frag != fragments.end();
                        ++frag ) {

                        frag->apply(doc,origin_x,origin_y);
                }
        }



#undef min
#undef max

}
}
