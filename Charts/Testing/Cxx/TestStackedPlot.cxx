/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestLinePlot.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkSmartPointer.h"
#include "vtkChartXY.h"
#include "vtkPlot.h"
#include "vtkAxis.h"
#include "vtkPlotStacked.h"
#include "vtkColor.h"
#include "vtkColorSeries.h"
#include "vtkTable.h"
#include "vtkIntArray.h"
#include "vtkDoubleArray.h"
#include "vtkStringArray.h"
#include "vtkContextView.h"
#include "vtkContextScene.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRegressionTestImage.h"



#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()


// Monthly checkout data
static const char *month_labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static int book[] =       {5675, 5902, 6388, 5990, 5575, 7393, 9878, 8082, 6417, 5946, 5526, 5166};
static int new_popular[] = {701,  687,  736,  696,  750,  814,  923,  860,  786,  735,  680,  741};
static int periodical[] =  {184,  176,  166,  131,  171,  191,  231,  166,  197,  162,  152,  143};
static int audiobook[] =   {903, 1038,  987, 1073, 1144, 1203, 1173, 1196, 1213, 1076,  926,  874};
static int video[] =      {1524, 1565, 1627, 1445, 1179, 1816, 2293, 1811, 1588, 1561, 1542, 1563};

//----------------------------------------------------------------------------
int TestStackedPlot( int argc, char * argv [] )
{
  // Set up a 2D scene, add an XY chart to it
  VTK_CREATE(vtkContextView, view);
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  view->GetRenderWindow()->SetSize(400, 300);
  VTK_CREATE(vtkChartXY, chart);
  view->GetScene()->AddItem(chart);

  // Create a table with some points in it...
  VTK_CREATE(vtkTable, table);

  VTK_CREATE(vtkStringArray, arrMonthLabel);
  arrMonthLabel->SetNumberOfValues(12);

  VTK_CREATE(vtkDoubleArray, arrXTickPositions);
  arrXTickPositions->SetNumberOfValues(12);

  VTK_CREATE(vtkIntArray, arrMonth);
  arrMonth->SetName("Month");
  table->AddColumn(arrMonth);

  VTK_CREATE(vtkIntArray, arrBook);
  arrBook->SetName("Books");
  table->AddColumn(arrBook);

  VTK_CREATE(vtkIntArray, arrNewPopularBook);
  arrNewPopularBook->SetName("New / Popular");
  table->AddColumn(arrNewPopularBook);

  VTK_CREATE(vtkIntArray, arrPeriodical);
  arrPeriodical->SetName("Periodical");
  table->AddColumn(arrPeriodical);

  VTK_CREATE(vtkIntArray, arrAudiobook);
  arrAudiobook->SetName("Audiobook");
  table->AddColumn(arrAudiobook);

  VTK_CREATE(vtkIntArray, arrVideo);
  arrVideo->SetName("Video");
  table->AddColumn(arrVideo);

  table->SetNumberOfRows(12);
  for (int i = 0; i < 12; i++)
    {
    arrMonthLabel->SetValue(i,month_labels[i]);
    arrXTickPositions->SetValue(i,i);

    arrBook->SetValue(i,book[i]);
    arrNewPopularBook->SetValue(i,new_popular[i]);
    arrPeriodical->SetValue(i,periodical[i]);
    arrAudiobook->SetValue(i,audiobook[i]);
    arrVideo->SetValue(i,video[i]);
    }

  // Set the Month Labels
  chart->GetAxis(1)->SetTickLabels(arrMonthLabel);
  chart->GetAxis(1)->SetTickPositions(arrXTickPositions);
  chart->GetAxis(1)->SetMaximum(11);

  // Add multiple line plots, setting the colors etc
  vtkPlotStacked *stack = 0;

  // Books
  stack = vtkPlotStacked::SafeDownCast(chart->AddPlot(vtkChart::STACKED));
  stack->SetUseIndexForXSeries(true);
  stack->SetInput(table);
  stack->SetInputArray(1,"Books");
  stack->SetInputArray(2,"New / Popular");
  stack->SetInputArray(3,"Periodical");
  stack->SetInputArray(4,"Audiobook");
  stack->SetInputArray(5,"Video");

  VTK_CREATE(vtkColorSeries,colorSeries);
  colorSeries->ClearColors();
  colorSeries->AddColor(vtkColor3ub(120,120,254));
  colorSeries->AddColor(vtkColor3ub(254,118,118));
  colorSeries->AddColor(vtkColor3ub(170,170,254));
  colorSeries->AddColor(vtkColor3ub(91,91,254));
  colorSeries->AddColor(vtkColor3ub(253,158,158));

  stack->SetColorSeries(colorSeries);
 
  //Finally render the scene and compare the image to a reference image
  view->GetRenderWindow()->SetMultiSamples(0);

  int retVal = vtkRegressionTestImageThreshold(view->GetRenderWindow(), 25);
  if(retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();
    }

  return !retVal;
}
