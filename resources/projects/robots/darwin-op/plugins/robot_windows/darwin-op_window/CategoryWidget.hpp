#ifndef CATEGORY_WIDGET_HPP
#define CATEGORY_WIDGET_HPP

#include <QtCore/QList>
#include <QtGui/QScrollArea>

class QGridLayout;
class QString;

class CategoryWidget : public QScrollArea
{
public:
  CategoryWidget(const QString &name);
  virtual ~CategoryWidget();

  const QString &name() const { return mName; }
  void addWidget(QWidget *widget);

private:
  QString mName;
  int mNWidgets;

  QWidget *mContainerWidget;
  QGridLayout *mContainerGridLayout;
};

#endif
