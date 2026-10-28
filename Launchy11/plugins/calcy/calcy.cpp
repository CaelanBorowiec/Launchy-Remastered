/*
  Launchy: Application Launcher
  Copyright (C) 2007-2010 Josh Karlin
  
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

#include "precompiled.h"
#include "calcy.h"

#include <cmath>
#include <limits>


namespace
{

// Same expressions the Boost.Spirit Classic grammar accepted:
//   expression = term *( '+' term | '-' term )
//   term       = factor *( '*' factor | '/' factor )
//   factor     = number | '(' expression ')' | '-' factor | '+' factor
// number is an unsigned real, including a fraction and an exponent (1.5e-2).
// A prefix that parses is kept even when trailing input is left over, and a
// query that does not start with an expression evaluates to 0.
class Calculator
{
public:
	explicit Calculator(const QString& text)
		: s(text), i(0)
	{
	}

	double evaluate()
	{
		double value = 0;
		if (!expression(value))
			return 0;
		return value;
	}

private:
	const QString& s;
	qsizetype i;

	static bool isDigit(QChar c)
	{
		return c >= QLatin1Char('0') && c <= QLatin1Char('9');
	}

	static int digit(QChar c)
	{
		return c.unicode() - QLatin1Char('0').unicode();
	}

	void skip()
	{
		while (i < s.size() && s[i].isSpace())
			++i;
	}

	bool eat(QChar c)
	{
		skip();
		if (i < s.size() && s[i] == c)
		{
			++i;
			return true;
		}
		return false;
	}

	bool expression(double& out)
	{
		double value = 0;
		if (!term(value))
			return false;

		for (;;)
		{
			const qsizetype save = i;
			if (eat(QLatin1Char('+')))
			{
				double rhs = 0;
				if (!term(rhs))
				{
					i = save;
					break;
				}
				value += rhs;
			}
			else if (eat(QLatin1Char('-')))
			{
				double rhs = 0;
				if (!term(rhs))
				{
					i = save;
					break;
				}
				value -= rhs;
			}
			else
				break;
		}

		out = value;
		return true;
	}

	bool term(double& out)
	{
		double value = 0;
		if (!factor(value))
			return false;

		for (;;)
		{
			const qsizetype save = i;
			if (eat(QLatin1Char('*')))
			{
				double rhs = 0;
				if (!factor(rhs))
				{
					i = save;
					break;
				}
				value *= rhs;
			}
			else if (eat(QLatin1Char('/')))
			{
				double rhs = 0;
				if (!factor(rhs))
				{
					i = save;
					break;
				}
				value /= rhs;
			}
			else
				break;
		}

		out = value;
		return true;
	}

	bool factor(double& out)
	{
		const qsizetype save = i;
		skip();

		double value = 0;
		if (number(value))
		{
			out = value;
			return true;
		}
		i = save;

		if (eat(QLatin1Char('(')))
		{
			if (expression(value) && eat(QLatin1Char(')')))
			{
				out = value;
				return true;
			}
			i = save;
			return false;
		}
		if (eat(QLatin1Char('-')))
		{
			if (factor(value))
			{
				out = -value;
				return true;
			}
			i = save;
			return false;
		}
		if (eat(QLatin1Char('+')))
		{
			if (factor(value))
			{
				out = value;
				return true;
			}
			i = save;
			return false;
		}
		return false;
	}

	// Unsigned real, as ureal_p parsed it. A leading or trailing dot is
	// allowed. An 'e'/'E' that is not followed by an exponent rejects the
	// whole number so the caller can backtrack.
	bool number(double& out)
	{
		const qsizetype start = i;
		qsizetype pos = i;
		const qsizetype n = s.size();

		qsizetype intDigits = 0;
		while (pos < n && isDigit(s[pos]))
		{
			++pos;
			++intDigits;
		}

		qsizetype fracDigits = 0;
		if (pos < n && s[pos] == QLatin1Char('.'))
		{
			++pos;
			while (pos < n && isDigit(s[pos]))
			{
				++pos;
				++fracDigits;
			}
		}

		if (intDigits == 0 && fracDigits == 0)
			return false;

		const qsizetype mantissaEnd = pos;
		double expSign = 1;
		double expAbs = 0;
		bool hasExp = false;
		if (pos < n && (s[pos] == QLatin1Char('e') || s[pos] == QLatin1Char('E')))
		{
			qsizetype expPos = pos + 1;
			if (expPos < n && (s[expPos] == QLatin1Char('+') || s[expPos] == QLatin1Char('-')))
			{
				if (s[expPos] == QLatin1Char('-'))
					expSign = -1;
				++expPos;
			}
			if (expPos >= n || !isDigit(s[expPos]))
				return false;

			const double maxExp = std::numeric_limits<double>::max() / 10.0;
			while (expPos < n && isDigit(s[expPos]))
			{
				const int d = digit(s[expPos]);
				if (expAbs > maxExp)
					return false;
				expAbs = expAbs * 10.0 + d;
				++expPos;
			}

			hasExp = true;
			pos = expPos;
		}

		bool ok = false;
		double value = s.mid(start, mantissaEnd - start).toDouble(&ok);
		if (!ok)
			return false;
		if (hasExp)
			value *= std::pow(10.0, expSign * expAbs);

		i = pos;
		out = value;
		return true;
	}
};

} // namespace


bool DoCalculation(QString str, double& result)
{
	Calculator calc(str);
	result = calc.evaluate();
	return true;
}


calcyPlugin* gPlugin;


calcyPlugin::calcyPlugin()
{
	gPlugin = this;
	HASH_CALCY = qHash(QString("calcy"));
}


calcyPlugin::~calcyPlugin()
{
}


void calcyPlugin::init()
{
	QString decimal = (*settings)->value("calcy/useCommaForDecimal", false).toBool() ? "," : ".";
	QString group = (*settings)->value("calcy/useCommaForDecimal", false).toBool() ? "." : ",";

	QString pattern = QString("^[\\(\\+\\-]*([\\d\\%1]?(\\%2\\d+)?)").arg(group).arg(decimal);
	reg.setPattern(pattern);
}


void calcyPlugin::getID(uint* id)
{
	*id = HASH_CALCY;
}


void calcyPlugin::getName(QString* str)
{
	*str = "Calcy";
}


void calcyPlugin::getLabels(QList<InputData>* id)
{
	if (id->count() > 1)
		return;

	QString text = id->last().getText().replace(" ", "");
	if (reg.indexIn(text) == 0)
	{
		id->last().setLabel(HASH_CALCY);
	}
}


void calcyPlugin::getResults(QList<InputData>* id, QList<CatItem>* results)
{   
	if (id->last().hasLabel(HASH_CALCY))
	{
		QString text = id->last().getText();
		double res = 0.0;

		QString decimal = (*settings)->value("calcy/useCommaForDecimal", false).toBool() ? "," : ".";
		QString group = (*settings)->value("calcy/useCommaForDecimal", false).toBool() ? "." : ",";

		QLocale c = (*settings)->value("calcy/useCommaForDecimal", false).toBool() ? QLocale(QLocale::German) : QLocale(QLocale::C);

		
		text = text.replace(group, "");
		text = text.replace(decimal, ".");
		

		//double dbl = c.toDouble(text);
		//qDebug() << text << dbl;
		//text = QString::number(dbl);

		if (!DoCalculation(text, res))
			return;

		QString szRes = c.toString(res, 'f', (*settings)->value("calcy/outputRounding", 10).toInt());

		// Remove any trailing fractional zeros
		if (szRes.contains(decimal))
		{
			while (szRes.endsWith("0"))
				szRes.chop(1);
			if (szRes.endsWith(decimal))
				szRes.chop(1);
		}
		results->push_front(CatItem(szRes + ".calcy", szRes, HASH_CALCY, getIcon()));
	}
}


void calcyPlugin::launchItem(QList<InputData>* /*inputData*/, CatItem* item)
{
	if ((*settings)->value("calcy/copyToClipboard", true).toBool())
	{
		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText(item->shortName);
	}
}


QString calcyPlugin::getIcon()
{
	return libPath + "/icons/calcy.png";
}


void calcyPlugin::setPath(QString * path)
{
	libPath = *path;
}


void calcyPlugin::doDialog(QWidget* parent, QWidget** newDlg)
{
	if (gui != NULL)
		return;
	gui.reset(new Gui(parent));
	*newDlg = gui.get();
	init();
}


void calcyPlugin::endDialog(bool accept)
{
	if (accept)
	{
		gui->writeOptions();
		init();
	}
	gui.reset();
}


int calcyPlugin::msg(int msgId, void* wParam, void* lParam)
{
	bool handled = false;
	switch (msgId)
	{		
	case MSG_INIT:
		init();
		handled = true;
		break;
	case MSG_GET_LABELS:
		getLabels((QList<InputData>*)wParam);
		handled = true;
		break;
	case MSG_GET_ID:
		getID((uint*) wParam);
		handled = true;
		break;
	case MSG_GET_NAME:
		getName((QString*)wParam);
		handled = true;
		break;
	case MSG_GET_RESULTS:
		getResults((QList<InputData>*) wParam, (QList<CatItem>*)lParam);
		handled = true;
		break;
	case MSG_LAUNCH_ITEM:
		launchItem((QList<InputData>*) wParam, (CatItem*) lParam);
		handled = true;
		break;
	case MSG_HAS_DIALOG:
		handled = true;
		break;
	case MSG_DO_DIALOG:
		doDialog((QWidget*) wParam, (QWidget**) lParam);
		break;
	case MSG_END_DIALOG:
		endDialog(wParam != 0);
		break;
	case MSG_PATH:
		setPath((QString*) wParam);	    
	default:
		break;
	}

	return handled;
}


Q_EXPORT_PLUGIN2(calcy, calcyPlugin) 
