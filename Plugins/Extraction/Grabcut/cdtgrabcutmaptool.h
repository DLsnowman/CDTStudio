#ifndef CDTGRABCUTMAPTOOL_H
#define CDTGRABCUTMAPTOOL_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef MARK_VALUE
#define MARK_VALUE 3
#endif

#include <qgsmaptool.h>

class QgsMapCanvas;
class QgsRubberBand;
class QgsVectorLayer;

class CDTGrabcutMapTool : public QgsMapTool
{
    Q_OBJECT
public:
    friend class GrabcutInterface;
    explicit CDTGrabcutMapTool(QgsMapCanvas *canvas);
    ~CDTGrabcutMapTool();

    virtual void canvasMoveEvent ( QgsMapMouseEvent * e );
    virtual void canvasPressEvent(QgsMapMouseEvent *e );
signals:

public slots:

private:
    QgsRubberBand   *mRubberBand;
    QString         imagePath;
    QgsVectorLayer  *vectorLayer;

};

#endif // CDTGRABCUTMAPTOOL_H
