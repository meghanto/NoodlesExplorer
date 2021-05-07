#ifndef CHARTVIEWER_H
#define CHARTVIEWER_H

#include "ui_chartviewer.h"

#include "noo_include_glm.h"

#include <QAbstractTableModel>
#include <QPointer>
#include <QPushButton>
#include <QWidget>

class ExTable;
class RemoteTableData;
class ChartViewer;

namespace QtCharts {
class QAbstractSeries;
class QChart;
class QValueAxis;
} // namespace QtCharts


class ColorWell : public QPushButton {
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged);

    QColor m_color;

public:
    ColorWell(QWidget* parent = nullptr);

    QColor color() const { return m_color; }
public slots:
    void setColor(QColor color);
signals:
    void colorChanged(QColor color);
};

struct ChartSeriesPart {
    QString name  = "Unnamed";
    int     type  = -1;
    int     a_col = -1;
    int     b_col = -1;
    QColor  color = Qt::black;

    QtCharts::QAbstractSeries* series;
    glm::vec2                  mins;
    glm::vec2                  maxs;
    void                       rebuild(ChartViewer& chart_view);
};


class SeriesTable : public QAbstractTableModel {
    ChartViewer*                 m_host;
    std::vector<ChartSeriesPart> m_active_series;

public:
    explicit SeriesTable(ChartViewer*);

public:
    QVariant headerData(int             section,
                        Qt::Orientation orientation,
                        int             role = Qt::DisplayRole) const override;

    int rowCount(QModelIndex const& parent = QModelIndex()) const override;
    int columnCount(QModelIndex const& parent = QModelIndex()) const override;

    QVariant data(QModelIndex const& index,
                  int                role = Qt::DisplayRole) const override;

    bool setData(QModelIndex const& index,
                 QVariant const&    value,
                 int                role = Qt::EditRole) override;

    Qt::ItemFlags flags(QModelIndex const& index) const override;

public:
    void clear();
    void rebuild_all();
    void recompute_bounds();

    void add_new();
    void del_at(int);
};

class ChartViewer : public QObject {
    Q_OBJECT

    QPointer<QWidget>              m_widget;
    std::unique_ptr<Ui::ChartRoot> m_ui_root;

    QtCharts::QChart*     m_chart;
    QtCharts::QValueAxis* m_x_axis;
    QtCharts::QValueAxis* m_y_axis;

    std::shared_ptr<ExTable>         m_attached_table;
    std::shared_ptr<RemoteTableData> m_data;

    std::vector<QMetaObject::Connection> m_table_obj_links;

    SeriesTable m_series_table;

    void setup_root();
    void setup_edit_page();
    void setup_chart_page();

public:
    explicit ChartViewer(std::shared_ptr<ExTable>, QObject* parent = nullptr);

    QtCharts::QChart*     chart() const { return m_chart; }
    QtCharts::QValueAxis* x_axis() const { return m_x_axis; }
    QtCharts::QValueAxis* y_axis() const { return m_y_axis; }
    RemoteTableData*      current_data() const { return m_data.get(); }

    void connect_table();

private slots:
    void on_table_changed();

signals:
};

#endif // CHARTVIEWER_H
