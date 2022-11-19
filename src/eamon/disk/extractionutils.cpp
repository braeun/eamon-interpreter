/********************************************************************************
 *                                                                              *
 * EamonInterpreter - extraction utilities for Apple ][ disk image readers      *
 *                                                                              *
 * modified: 2022-11-15                                                         *
 *                                                                              *
 ********************************************************************************
 * Copyright (C) Harald Braeuning                                               *
 ********************************************************************************
 * This file is part of EamonInterpreter.                                       *
 * EamonInterpreter is free software: you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the Free   *
 * Software Foundation, either version 3 of the License, or (at your option)    *
 * any later version.                                                           *
 * EamonInterpreter is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                                *
 * You should have received a copy of the GNU General Public License along with *
 * EamonInterpreter. If not, see <https://www.gnu.org/licenses/>.               *
 ********************************************************************************/

#include "extractionutils.h"
#include "dosdisk.h"
#include <QDir>
#include <QMessageBox>
#include <vector>

/*
 *  Screen display mapping table
 */
const QChar ExtractionUtils::screen_map[] = {
  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',		/* $00	*/
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',		/* $08	*/
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',		/* $10	*/
  'X', 'Y', 'Z', '[', '\\',']', '^', '_',		/* $18	*/
  ' ', '!', '"', '#', '$', '%', '&', '\'',	/* $20	*/
  '(', ')', '*', '+', ',', '-', '.', '/',		/* $28	*/
  '0', '1', '2', '3', '4', '5', '6', '7',		/* $30	*/
  '8', '9', ':', ';', '<', '=', '>', '?',		/* $38	*/

  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',		/* $40	*/
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',		/* $48	*/
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',		/* $50	*/
  'X', 'Y', 'Z', '[', '\\',']', '^', '_',		/* $58	*/
  ' ', '!', '"', '#', '$', '%', '&', '\'',	/* $60	*/
  '(', ')', '*', '+', ',', '-', '.', '/',		/* $68	*/
  '0', '1', '2', '3', '4', '5', '6', '7',		/* $70	*/
  '8', '9', ':', ';', '<', '=', '>', '?',		/* $78	*/

  '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g',		/* $80	*/
  'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',		/* $88	*/
  'p', 'q', 'r', 's', 't', 'u', 'v', 'w',		/* $90	*/
  'x', 'y', 'z', '[', '\\',']', '^', '_',		/* $98	*/
  ' ', '!', '"', '#', '$', '%', '&', '\'',	/* $A0	*/
  '(', ')', '*', '+', ',', '-', '.', '/',		/* $A8	*/
  '0', '1', '2', '3', '4', '5', '6', '7',		/* $B0	*/
  '8', '9', ':', ';', '<', '=', '>', '?',		/* $B8	*/

  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',		/* $C0	*/
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',		/* $C8	*/
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',		/* $D0	*/
  'X', 'Y', 'Z', '[', '\\',']', '^', '_',		/* $D8	*/
  ' ', '!', '"', '#', '$', '%', '&', '\'',	/* $E0	*/
  '(', ')', '*', '+', ',', '-', '.', '/',		/* $E8	*/
  '0', '1', '2', '3', '4', '5', '6', '7',		/* $F0	*/
  '8', '9', ':', ';', '<', '=', '>', '?',		/* $F8	*/
};

static std::vector<const char*> integerBasicTokens{
  nullptr,	 	nullptr, 		nullptr,	":",		"LOAD ",	"SAVE ",	nullptr, 		"RUN ",	// $00-$07
  nullptr,		"DEL ",		", ",	"NEW ",		"CLR ",		"AUTO ",	nullptr,		"MAN ",	// $08-$0F
  "HIMEM:",	"LOMEM:",	"+",	"-",		"*",		"/",		"=",		"#",	// $10-$17
  ">=",		">",		"<=",	"<>",		"<",		" AND ",	" OR ",		" MOD ",// $18-$1F
  "^",		nullptr,		"(",	",",		" THEN ",	" THEN ",	",",		",",	// $20-$27
  "\"",		"\"",		"(",	nullptr,		nullptr,		"(",		" PEEK ",	" RND ",// $28-$2F
  "SGN ",		"ABS ",		"PDL ",	nullptr,		"(",		"+",		"-",		"NOT ",	// $30-$37
  "(",		"=",		"#",	" LEN(",	" ASC(",	" SCRN(",	",",		"(",	// $38-$3F
  "$",		nullptr,		"(",	",",		",",		";",		";",		";",	// $40-$47
  ",",		",",		",",	"TEXT ",	"GR ",		"CALL ",	"DIM ",		"DIM ",	// $48-$4F
  "TAB ",		"END ",		"INPUT ",	"INPUT ",	"INPUT ",	"FOR ",	"=",		" TO ",	// $50-$57
  " STEP ",	"NEXT ",	",",	"RETURN ",	"GOSUB ",	"REM ",		"LET ",		"GOTO ",// $58-$5F
  "IF ",		"PRINT ",	"PRINT ",	"PRINT ",	" POKE ",	",",	"COLOR= ",	"PLOT ",// $60-$67
  ",",		"HLIN ",	",",	" AT ",		"VLIN ",	",",		" AT ",		"VTAB ",// $68-$6F
  "=",		"=",		")",	nullptr,		"LIST ",	",",		nullptr,		"POP ",	// $70-$77
  nullptr,		"NO DSP ",	"NO TRACE ",	"DSP ",	"DSP ",	"TRACE ",	"PR # ",	"IN # "	// $78-$7F
};


static std::vector<const char*> applesoftTokens{	// starts at $80
  " END ",		" FOR ",	" NEXT ",	" DATA ",	" INPUT ",		" DEL ",
  " DIM ",		" READ ",	" GR ",		" TEXT ",	" PR# ",		" IN# ",
  " CALL ",		" PLOT ",	" HLIN ",	" VLIN ",	" HGR2 ",		" HGR ",
  " HCOLOR= ",	" HPLOT ",	" DRAW ",	" XDRAW ",	" HTAB ",		" HOME ",
  " ROT= ",		" SCALE= ",	" SHLOAD ",	" TRACE ",	" NOTRACE ",	" NORMAL ",
  " INVERSE ",	" FLASH ",	" COLOR= ",	" POP ",	" VTAB ",		" HIMEM: ",
  " LOMEM: ",		" ONERR ",	" RESUME ",	" RECALL ",	" STORE ",		" SPEED= ",
  " LET ",		" GOTO ",	" RUN ",	" IF ",		" RESTORE ",	" & ",
  " GOSUB ",		" RETURN ",	" REM ",	" STOP ",	" ON ",			" WAIT ",
  " LOAD ",		" SAVE ",	" DEF ",	" POKE ",	" PRINT ",		" CONT ",
  " LIST ",		" CLEAR ",	" GET ",	" NEW ",	" TAB( ",		" TO ",
  " FN ",			" SPC( ",	"  THEN ",	" AT ",		"  NOT ",		"  STEP ",
  " +",			" -",		" *",		"/",		" ^",			"  AND ",
  "  OR ",		" >",		" = ",		" <",		" SGN",			" INT",
  " ABS",			" USR",		" FRE",		" SCRN( ",	" PDL",			" POS",
  " SQR",			" RND",		" LOG",		" EXP",		" COS",			" SIN",
  " TAN",			" ATN",		" PEEK",	" LEN",		" STR$",		" VAL",
  " ASC",			" CHR$",	" LEFT$",	" RIGHT$",	" MID$ " };



ExtractionUtils::ExtractionUtils()
{
}

QChar ExtractionUtils::getAscii(uint8_t c)
{
  return QChar(c&0x7F);
//  return screen_map[c];
}

uint32_t ExtractionUtils::getShortLH(const QByteArray &a, uint32_t offset)
{
  return static_cast<uint8_t>(a[offset]) + 256 * static_cast<uint8_t>(a[offset+1]);
}

QString ExtractionUtils::convertText(const QByteArray &a)
{
  char txt[a.size()+1];
  int32_t j = 0;
  for (int32_t i=0;i<a.size();i++)
  {
    if (static_cast<uint8_t>(a[i]) != 0)
    {
      txt[j++] = a[i] & 0x7F; //ExtractionUtils::getAscii(static_cast<uint8_t>(data[i])).toLatin1();
    }
  }
  txt[j] = '\0';
  return txt;
}

QString ExtractionUtils::convertTextKeepLength(const QByteArray &a)
{
  char txt[a.size()+1];
  int32_t j = 0;
  for (int32_t i=0;i<a.size();i++)
  {
    if (static_cast<uint8_t>(a[i]) != 0)
    {
      txt[j++] = a[i] & 0x7F; //ExtractionUtils::getAscii(static_cast<uint8_t>(data[i])).toLatin1();
    }
    else
    {
      txt[j++] = ' ';
    }
  }
  txt[j] = '\0';
  return txt;
}

QByteArray ExtractionUtils::convertByteArray(const QByteArray &a)
{
  QByteArray r(a);
  for (int32_t i=0;i<r.size();i++)
  {
    if (static_cast<uint8_t>(r[i]) != 0)
    {
      r[i] = r[i] & 0x7F; //ExtractionUtils::getAscii(static_cast<uint8_t>(data[i])).toLatin1();
    }
  }
  return r;
}

QString ExtractionUtils::convertApplesoftBasic(const QByteArray &data)
{
  int32_t offset = 0;
  QString txt = "";
  int32_t linenr = -1;
  while (offset < data.size())
  {
    if (linenr < 0) /* new line */
    {
      if (ExtractionUtils::getShortLH(data,offset) == 0) break; /* end of program */
      offset += 2;
      linenr = ExtractionUtils::getShortLH(data,offset);
      offset += 2;
      txt += QString::number(linenr) + " ";
    }
    uint8_t b = static_cast<uint8_t>(data[offset++]);
    if (b >= 0x80)
    {
      uint32_t index = b & 0x7F;
      if (index >= applesoftTokens.size())
        txt += "<unknown token>";
      else
        txt += applesoftTokens[index];
    }
    else if (b == '\"')
    {
      txt += QChar(b);
      do
      {
        b = static_cast<uint8_t>(data[offset++]);
        if (b == 0)
        {
          linenr = -1;
          txt += "\n";
          break;
        }
        else if (b < 0x20)
        {
          txt += "<CTRL-";
          txt += '@' + b;
          txt += ">";
        }
        else
        {
          txt += QChar(b);
        }
      }
      while (b != '\"');
    }
    else if (b > 0)
    {
      if (b < 0x20)
      {
        txt += "<CTRL-";
        txt += '@' + b;
        txt += ">";
      }
      else
      {
        txt += QChar(b);
      }
    }
    else // if (b == 0)
    {
      linenr = -1;
      txt += "\n";
    }
  }
  return txt;
}

QString ExtractionUtils::convertIntegerBasic(const QByteArray &data)
{
  int32_t offset = 0;
  QString txt = "";
  int32_t linenr = -1;
  uint8_t linelength;
  bool comment = false;
  bool literal = false;
  while (offset < data.size())
  {
    if (linenr < 0) /* new line */
    {
      linelength = static_cast<uint8_t>(data[offset]);
      if (linelength == 0) break; /* end of program */
      if (static_cast<uint8_t>(data[offset+linelength-1]) != 0x01)
      {
        txt += "ERROR: invalid line length";
        break;
      }
      linenr = ExtractionUtils::getShortLH(data,offset+1);
      offset += 3;
      txt += QString::number(linenr) + " ";
    }
    uint8_t b = static_cast<uint8_t>(data[offset++]);
    if (b == 0x01)
    {
      comment = false;
      literal = false;
      linenr = -1;
      txt += "\n";
    }
    else if (comment || literal)
    {
      if (literal && b == 0x29)
      {
        literal = false;
        txt += integerBasicTokens[b];
      }
      else
      {
        char c = b & 0x7F;
        if (c < 0x20)
        {
          txt += "<CTRL-";
          txt += '@' + c;
          txt += ">";
        }
        else
        {
          txt += QChar(c);
        }
      }
    }
    else if ((b & 0x80) == 0)
    {
      if (integerBasicTokens[b]) txt += integerBasicTokens[b];
      comment = (b == 0x5d);	// REM statement
      literal = (b == 0x28);	// open quote
    }
    else
    {
      if (b >= 0xB0 && b <= 0xB9) /* numeric constant */
      {
        uint32_t v = ExtractionUtils::getShortLH(data,offset);
        txt += QString::number(v);
        offset += 2;
      }
      else /* identifier */
      {
        txt += QChar(b&0x7F);
      }
    }
  }
  return txt;
}

/**
 * Bit masks used for the bit shifting or testing operations.
 */
static uint8_t masks[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

QImage ExtractionUtils::convertHiresBW(const QByteArray &data, uint32_t foreground, uint32_t background)
{
  uint32_t* bitmap = new uint32_t[280*192];
  for (int32_t y=0;y<192;y++)
  {
    int32_t base = (			// odd notation - bit value shifted right * hex value
      ((y & 0x7) << 10)			// 00000111 * 0x0400
      | (y & 0x8) << 4			// 00001000 * 0x0080
      | (y & 0x30) << 4			// 00110000 * 0x0100
      | ((y & 0xc0) >> 6) * 0x028	// 11000000 * 0x0028
      ) & 0x1fff;
    for (int x=0; x<280; x++)
    {
      uint8_t b = data[base+x/7];
      if (b & masks[x%7])
        bitmap[y*280+x] = foreground;
      else
        bitmap[y*280+x] = background;
    }
  }
  return QImage(reinterpret_cast<uint8_t*>(bitmap),280,192,QImage::Format_RGB32);
}

QImage ExtractionUtils::convertHiresBW(const std::vector<uint8_t> &data, uint32_t foreground, uint32_t background)
{
  uint32_t* bitmap = new uint32_t[280*192];
  for (int32_t y=0;y<192;y++)
  {
    int32_t base = (			// odd notation - bit value shifted right * hex value
      ((y & 0x7) << 10)			// 00000111 * 0x0400
      | (y & 0x8) << 4			// 00001000 * 0x0080
      | (y & 0x30) << 4			// 00110000 * 0x0100
      | ((y & 0xc0) >> 6) * 0x028	// 11000000 * 0x0028
      ) & 0x1fff;
    for (int x=0; x<280; x++)
    {
      uint8_t b = data[base+x/7];
      if (b & masks[x%7])
        bitmap[y*280+x] = foreground;
      else
        bitmap[y*280+x] = background;
    }
  }
  return QImage(reinterpret_cast<uint8_t*>(bitmap),280,192,QImage::Format_RGB32);
}

/**
 * Given a specific line in the image, process it in hires color mode.
 * HGR color is two bits to determine color - essentially resolution is
 * 140 horizontally, but it indicates the color for two pixels.
 * <p>
 * The names of pixels is a bit confusion - pixel0 is really the left-most
 * pixel (not the low-value bit).
 * To alleviate my bad naming, here is a color table to assist:<br>
 * <pre>
 * Color   Bits      RGB
 * ======= ==== ========
 * Black1   000 0x000000
 * Green    001 0x00ff00
 * Violet   010 0xff00ff
 * White1   011 0xffffff
 * Black2   100 0x000000
 * Orange   101 0xff8000
 * Blue     110 0x0000ff
 * White2   111 0xffffff
 * </pre>
 * Remember: bits are listed as "highbit", "pixel0", "pixel1"!
 */
QImage ExtractionUtils::convertHiresColor(const QByteArray &data)
{
  uint32_t* bitmap = new uint32_t[280*192];
  for (int32_t y=0;y<192;y++)
  {
    int32_t base = (			// odd notation - bit value shifted right * hex value
      ((y & 0x7) << 10)			// 00000111 * 0x0400
      | (y & 0x8) << 4			// 00001000 * 0x0080
      | (y & 0x30) << 4			// 00110000 * 0x0100
      | ((y & 0xc0) >> 6) * 0x028	// 11000000 * 0x0028
      ) & 0x1fff;
    for (int x0=0;x0<280;x0+=2)
    {
      int x1 = x0 + 1;
      int offset0 = x0 / 7;	// byte across row
      int bit0 = x0 % 7;		// bit to test
      bool pixel0 = (data[base+offset0] & masks[bit0]) != 0;
      int offset1 = x1 / 7;	// byte across row
      int bit1 = x1 % 7;		// bit to test
      bool pixel1 = (data[base+offset1] & masks[bit1]) != 0;
      uint32_t color;
      if (pixel0 && pixel1)
        color = 0xFFFFFFFF;	// white
      else if (!pixel0 && !pixel1)
        color = 0;			// black
      else
      {
        bool highbit = pixel0 ? data[base+offset0]&masks[7] : data[base+offset1]&masks[7];
        if (pixel0 && highbit)
          color = 0xFF0000FF;	// blue
        else if (pixel0 && !highbit)
          color = 0xFFFF00FF;	// violet
        else if (pixel1 && !highbit)
          color = 0xFF00FF00;	// green
        else // pixel1 && highbit
          color = 0xFFFF8000;	// orange
      }
      bitmap[y*280+x0] = color;
      bitmap[y*280+x1] = color;
    }
  }
  return QImage(reinterpret_cast<uint8_t*>(bitmap),280,192,QImage::Format_RGB32);
}

bool ExtractionUtils::extractDisk(const QString &diskimg, const QString &dirname)
{
  AbstractDisk* disk = new DosDisk(diskimg);
  QDir dir(dirname);
  if (!dir.exists()) dir.mkpath(dir.absolutePath());
  for (const auto& e : disk->getDirectory())
  {
    QString filename = e.filename.toLower().trimmed();
    filename.replace(QRegExp("[ ':/]"),"_");
    QByteArray data = disk->getFileData(e,false);
    switch (e.type)
    {
      case DirectoryEntry::TEXT:
        saveByteArray(dir.absoluteFilePath(filename),convertByteArray(data));
        break;
      case DirectoryEntry::APPLESOFT_BASIC:
        saveText(dir.absoluteFilePath(filename),convertApplesoftBasic(data));
        break;
      case DirectoryEntry::INTEGER_BASIC:
        saveText(dir.absoluteFilePath(filename),convertIntegerBasic(data));
        break;
      case DirectoryEntry::BINARY:
        saveByteArray(dir.absoluteFilePath(filename),data);
        break;
    }
  }
  return true;
}




void ExtractionUtils::saveText(const QString &filename, const QString &text)
{
  QFile file(filename);
  if (file.open(QFile::WriteOnly|QFile::Text))
  {
    file.write(text.toLatin1());
    file.close();
  }
}

void ExtractionUtils::saveByteArray(const QString &filename, const QByteArray &a)
{
  QFile file(filename);
  if (file.open(QFile::WriteOnly|QFile::Text))
  {
    file.write(a);
    file.close();
  }
}

