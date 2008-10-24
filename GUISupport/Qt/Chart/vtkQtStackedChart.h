/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkQtStackedChart.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  Copyright 2008 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
  the U.S. Government retains certain rights in this software.
-------------------------------------------------------------------------*/

/// \file vtkQtStackedChart.h
/// \date February 27, 2008

#ifndef _vtkQtChartStackedLayer_h
#define _vtkQtChartStackedLayer_h

#include "vtkQtChartExport.h"
#include "vtkQtChartSeriesLayer.h"

class vtkQtStackedChartInternal;
class vtkQtStackedChartOptions;
class vtkQtStackedChartSeriesOptions;


/// \class vtkQtStackedChart
/// \brief
///   The vtkQtStackedChart class is used to display a stacked chart.
class VTKQTCHART_EXPORT vtkQtStackedChart : public vtkQtChartSeriesLayer
{
  Q_OBJECT

public:
  enum {Type = vtkQtChart_StackedChartType};

public:
  vtkQtStackedChart();
  virtual ~vtkQtStackedChart();

  /// \name Setup Methods
  //@{
  virtual void setChartArea(vtkQtChartArea *area);

  virtual void setModel(vtkQtChartSeriesModel *model);
  //@}

  /// \name Drawing Parameters
  //@{
  /// \brief
  ///   Gets the stacked chart drawing options.
  /// \return
  ///   A pointer to the stacked chart drawing options.
  vtkQtStackedChartOptions *getOptions() const {return this->Options;}

  /// \brief
  ///   Sets the stacked chart drawing options.
  ///
  /// This method sets all the options at once, which can prevent
  /// unnecessary view updates.
  ///
  /// \param options The new stacked chart drawing options.
  void setOptions(const vtkQtStackedChartOptions &options);

  /// \brief
  ///   Gets the stacked chart series options.
  /// \param series The series index.
  /// \return
  ///   A pointer to the stacked chart series options.
  vtkQtStackedChartSeriesOptions *getStackedSeriesOptions(int series) const;
  //@}

  /// \name Layout Methods
  //@{
  virtual void getLayerDomain(vtkQtChartLayerDomain &domain) const;

  virtual void layoutChart(const QRectF &area);

  virtual bool drawItemFilter(QGraphicsItem *item, QPainter *painter);

  virtual bool getHelpText(const QPointF &point, QString &text);
  //@}

  /// \name Selection Methods
  //@{
  virtual void getSeriesAt(const QPointF &point,
      vtkQtChartSeriesSelection &selection) const;

  virtual void getPointsAt(const QPointF &point,
      vtkQtChartSeriesSelection &selection) const;

  virtual void getSeriesIn(const QRectF &area,
      vtkQtChartSeriesSelection &selection) const;

  virtual void getPointsIn(const QRectF &area,
      vtkQtChartSeriesSelection &selection) const;
  //@}

  /// \name QGraphicsItem Methods
  //@{
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
    QWidget *widget=0);
  //@}

public slots:
  /// \brief
  ///   Refreshes the stacked chart data from the model.
  ///
  /// The currently displayed data is cleaned up. If a model is set,
  /// it is used to populate the stacked chart.
  void reset();

protected:
  /// \brief
  ///   Creates a new stacked chart series options object.
  /// \param parent The parent object.
  /// \return
  ///   A pointer to the new stacked chart series options object.
  virtual vtkQtChartSeriesOptions *createOptions(QObject *parent);

  /// \brief
  ///   Sets up the series options defaults.
  /// \param options The new series options object.
  virtual void setupOptions(vtkQtChartSeriesOptions *options);

private slots:
  /// \brief
  ///   Prepares the stacked chart for a series insertion.
  /// \param first The first series index to be added.
  /// \param last The last series index to be added.
  void prepareSeriesInsert(int first, int last);

  /// \brief
  ///   Inserts stacked chart series at the given indexes.
  /// \param first The first series index to be added.
  /// \param last The last series index to be added.
  void insertSeries(int first, int last);

  /// \brief
  ///   Removes the specified series from the chart.
  /// \param first The first series index to be removed.
  /// \param last The last series index to be removed.
  void startSeriesRemoval(int first, int last);

  /// \brief
  ///   Finishes the domain changes after removing the series.
  /// \param first The first series index to be removed.
  /// \param last The last series index to be removed.
  void finishSeriesRemoval(int first, int last);

  /// Requests a chart layout when the axis corner option changes.
  void handleAxesCornerChange();

  /// Updates the chart when the sumation type has changed.
  void handleSumationChange();

  /// Updates the chart when the gradient option has changed.
  void handleGradientChange();

  /// \brief
  ///   Changes the series visibility.
  ///
  /// The signal sender is used to determine which series has changed.
  ///
  /// \param visible True if the series should be shown.
  void handleSeriesVisibilityChange(bool visible);

  /// \brief
  ///   Changes the series pen.
  /// \param pen The new series pen.
  void handleSeriesPenChange(const QPen &pen);

  /// \brief
  ///   Changes the series brush.
  /// \param brush The new series brush.
  void handleSeriesBrushChange(const QBrush &brush);

  /// \brief
  ///   Called to layout the highlights.
  ///
  /// The layout request is ignored if the model is being changed.
  void updateHighlights();

private:
  /// Called to layout the highlights.
  virtual void layoutHighlights();

  /// \brief
  ///   Adds the domain for the given series to the current domain.
  /// \param series The series index
  /// \param seriesGroup Used to return the domain group.
  virtual void addSeriesDomain(int series, int *seriesGroup);

  /// \brief
  ///   Updates the series table index map.
  /// \param seriesGroup The domain group to update.
  virtual void updateItemMap(int seriesGroup);

  /// \brief
  ///   Creates the table for the given series domain group.
  /// \param seriesGroup The domain group index.
  virtual void createTable(int seriesGroup);

  /// \brief
  ///   Normalizes the table for the given series domain group.
  /// \param seriesGroup The domain group index.
  virtual void normalizeTable(int seriesGroup);

  /// \brief
  ///   Calculates the x-axis domain for the given domain group.
  /// \param seriesGroup The domain group index.
  virtual void calculateXDomain(int seriesGroup);

  /// \brief
  ///   Calculates the y-axis domain for the given domain group.
  /// \param seriesGroup The domain group index.
  virtual void calculateYDomain(int seriesGroup);

  /// \brief
  ///   Finds the closest point index to the given location.
  /// \param polygon The stacked series polygon.
  /// \param point The location to search.
  virtual int findClosestIndex(const QPolygonF &polygon,
      const QPointF &point) const;

private:
  vtkQtStackedChartInternal *Internal; ///< Stores the series.
  vtkQtStackedChartOptions *Options;   ///< Stores the drawing options.
  bool InModelChange;                  ///< Used for selection changes.
};

#endif