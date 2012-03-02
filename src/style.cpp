/**
  *  This file is part of Subtivals.
  *
  *  Subtivals is free software: you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation, either version 3 of the License, or
  *  (at your option) any later version.
  *
  *  Subtivals is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>
  **/
#include <QtGui/QPainter>
#include <QtGui/QTextDocument>
#include <QtGui/QAbstractTextDocumentLayout>

#include "style.h"
#include "event.h"

Style::Style(const QString &p_line, QObject *p_parent) :
    QObject(p_parent),
    m_alignment(Qt::AlignVCenter)
{
    QList<QString> subparts = p_line.split(',');
    m_name = subparts[0];
    m_font = QFont(subparts[1], subparts[2].toInt());
    QString c = subparts[3].right(6);
    m_primaryColour.setBlue(c.mid(0, 2).toInt(0, 16));
    m_primaryColour.setGreen(c.mid(2, 2).toInt(0, 16));
    m_primaryColour.setRed(c.mid(4, 2).toInt(0, 16));
    m_marginL = subparts[19].toInt();
    m_marginR = subparts[20].toInt();
    m_marginV = subparts[21].toInt();

    // Alignment after the layout of the numpad (1-3 sub, 4-6 mid, 7-9 top)
    int alignment = subparts[18].right(1).toInt();
    if (alignment <= 3)
        m_alignment = Qt::AlignBottom;
    if (alignment >= 7)
        m_alignment = Qt::AlignTop;
    if (alignment % 3 == 0)
        m_alignment |= Qt::AlignRight;
    if (alignment % 3 == 1)
        m_alignment |= Qt::AlignLeft;
    if (alignment % 3 == 2)
        m_alignment |= Qt::AlignHCenter;
}

Style::Style(const Style &p_oth, const QFont& f, QObject *p_parent):
    QObject(p_parent),
    m_name(p_oth.m_name),
    m_font(f),
    m_primaryColour(p_oth.m_primaryColour),
    m_alignment(p_oth.m_alignment),
    m_marginL(p_oth.m_marginL),
    m_marginR(p_oth.m_marginR),
    m_marginV(p_oth.m_marginV)
{
}

const QString &Style::name() const {
    return m_name;
}

const QFont &Style::font() const {
    return m_font;
}

void Style::setFont(const QFont& f) {
    m_font = f;
}

const QColor &Style::primaryColour() const {
    return m_primaryColour;
}

void Style::setPrimaryColour(const QColor &c)
{
    m_primaryColour = c;
}

void Style::drawEvent(QPainter *painter, const Event &event, const QRect &bounds) const
{
    QString html;
    if (m_alignment & Qt::AlignLeft) {
        html = "<p align=\"left\">";
    } else if (m_alignment & Qt::AlignRight) {
        html = "<p align=\"right\">";
    } else {
        html = "<p align=\"center\">";
    }
    html = html.append(event.text());
    html = html.append("</p>");
    painter->setFont(m_font);
    painter->setPen(m_primaryColour);
    QTextDocument doc;
    doc.setHtml(html);
    doc.setDefaultFont(m_font);
    QAbstractTextDocumentLayout* layout = doc.documentLayout();
    QRect final(bounds);
    int marginL = m_marginL + event.marginL();
    int marginR = m_marginL + event.marginR();
    int marginV = m_marginL + event.marginV();
    final = final.adjusted(marginL, marginV, -marginR, marginV);
    doc.setPageSize(QSize(final.width(), final.height()));
    QAbstractTextDocumentLayout::PaintContext context;
    context.palette.setColor(QPalette::Text, painter->pen().color());
    painter->save();
    painter->translate(final.x(), final.y());
    layout->draw(painter, context);
    painter->restore();
}

