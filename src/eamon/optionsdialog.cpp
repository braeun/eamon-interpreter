/********************************************************************************
 *                                                                              *
 * EamonInterpreter - preferences dialog                                        *
 *                                                                              *
 * modified: 2023-02-17                                                         *
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

#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "defines.h"
#include "palettefactory.h"
#include <QDebug>
#include <QStyleFactory>

OptionsDialog::OptionsDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::OptionsDialog)
{
  ui->setupUi(this);
  ui->styleBox->insertItems(0,QStyleFactory::keys());
  for (const QString& key : PaletteFactory::getPaletteNames())
  {
    ui->paletteBox->addItem(key);
  }
  QSettings settings;
  updateFields(settings);
}

OptionsDialog::~OptionsDialog()
{
  delete ui;
}

void OptionsDialog::commit()
{
  QSettings settings;
  commitFields(settings);
}

void OptionsDialog::commitFields(QSettings &settings)
{
  QFont font;
  font = ui->fontBox->currentFont();
  font.setPointSize(ui->fontSizeBox->currentText().toInt());
  settings.setValue(SETTING_SCREEN_WIDTH,ui->widthBox->value());
  settings.setValue(SETTING_SCREEN_HEIGHT,ui->heightBox->value());
  settings.setValue(SETTING_SCREEN_FONT,font.toString());
  settings.setValue(SETTING_SCREEN_LOWERCASE,ui->lowerCaseBox->isChecked());
  settings.setValue(SETTING_SCREEN_CAPITALIZE,ui->capitalizeBox->isChecked());
  settings.setValue(SETTING_SCREEN_TEXT_COLOR,ui->textColorButton->color());
  settings.setValue(SETTING_SCREEN_BKG_COLOR,ui->bkgColorButton->color());
  settings.setValue(SETTING_SCREEN_LINESPACING,ui->linespacingBox->value());
  settings.setValue(SETTING_STYLE_STYLE,ui->styleBox->currentText());
  settings.setValue(SETTING_STYLE_PALETTE,ui->paletteBox->currentText());
  QString style = settings.value(SETTING_STYLE_STYLE,SETTING_VALUE_STYLE_STYLE).toString();
  QApplication::setStyle(QStyleFactory::create(style));
  QString palette = settings.value(SETTING_STYLE_PALETTE,SETTING_VALUE_STYLE_PALETTE).toString();
  QApplication::setPalette(PaletteFactory::getPalette(palette));
  settings.setValue(SETTING_VM_SLOWDOWN,ui->slowdownBox->value());
  settings.setValue(SETTING_AUTOSTART,ui->autostartBox->isChecked());
}

void OptionsDialog::updateFields(const QSettings &settings)
{
  QString s;

  ui->widthBox->setValue(settings.value(SETTING_SCREEN_WIDTH,SETTING_VALUE_SCREEN_WIDTH).toInt());
  ui->heightBox->setValue(settings.value(SETTING_SCREEN_HEIGHT,SETTING_VALUE_SCREEN_HEIGHT).toInt());
  s = settings.value(SETTING_SCREEN_FONT,SETTING_VALUE_SCREEN_FONT).toString();
  if (!s.isEmpty())
  {
    QFont font;
    font.fromString(s);
    ui->fontBox->setCurrentFont(font);
    ui->fontSizeBox->setCurrentText(QString::number(font.pointSize()));
  }
  ui->lowerCaseBox->setChecked(settings.value(SETTING_SCREEN_LOWERCASE,SETTING_VALUE_SCREEN_LOWERCASE).toBool());
  ui->capitalizeBox->setChecked(settings.value(SETTING_SCREEN_CAPITALIZE,SETTING_VALUE_SCREEN_CAPITALIZE).toBool());
  ui->textColorButton->setColor(settings.value(SETTING_SCREEN_TEXT_COLOR,SETTING_VALUE_SCREEN_TEXT_COLOR).value<QColor>());
  ui->bkgColorButton->setColor(settings.value(SETTING_SCREEN_BKG_COLOR,SETTING_VALUE_SCREEN_BKG_COLOR).value<QColor>());
  ui->linespacingBox->setValue(settings.value(SETTING_SCREEN_LINESPACING,SETTING_VALUE_SCREEN_LINESPACING).toInt());
  ui->styleBox->setCurrentText(settings.value(SETTING_STYLE_STYLE,SETTING_VALUE_STYLE_STYLE).toString());
  ui->paletteBox->setCurrentText(settings.value(SETTING_STYLE_PALETTE,SETTING_VALUE_STYLE_PALETTE).toString());
  ui->slowdownBox->setValue(settings.value(SETTING_VM_SLOWDOWN,SETTING_VALUE_VM_SLOWDOWN).toInt());
  ui->autostartBox->setChecked(settings.value(SETTING_AUTOSTART,SETTING_VALUE_AUTOSTART).toBool());
}
