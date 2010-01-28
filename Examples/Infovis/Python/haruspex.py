############################################################
from vtk import *
import sys
import getopt
############################################################

############################################################
# Parse command line
def ParseCommandLine():
    opts,args = getopt.getopt(sys.argv[1:], 'vd:e:a:s:v:h')

    # Default values
    verbosity = 0
    outModelPrefix = "outputModel"
    outDataName = "outputData.csv"
    
    # First verify that the helper has not been requested (supersedes everything else)
    # NB: handled first and separately so default values cannot be altered in helper message
    for o,a in opts:
        if o == "-h":
            Usage( outModelPrefix, outDataName, verbosity )

    # Parse arguments and assign corresponding variables
    for o,a in opts:
        if o == "-d":
            inDataName = a
        elif o == "-e":
            haruspexName = a
        elif o == "-a":
            outDataName = a
        elif o == "-s":
            outModelPrefix = a
        elif o == "-v":
            verbosity += 1

    if not inDataName:
        print "ERROR: a data file name required!"
        sys.exit( 1 )
        
    if not haruspexName:
        print "ERROR: a statistics engine name is required!"
        sys.exit( 1 )

    if verbosity > 0:
        print "# Parsed command line:"
        print "  Input data file:", inDataName
        print "  Statistics:", haruspexName
        print "  Output model file prefix:", outModelPrefix
        print "  Output data file:", outDataName
        print

    return [ inDataName, haruspexName, outModelPrefix, outDataName, verbosity ]
############################################################

############################################################
# Turn haruspex name into vtkStatistics object and ancillary parameters
def InstantiateStatistics( haruspexName, verbosity ):
    if haruspexName == "descriptive":
        haruspex = vtkDescriptiveStatistics()
        numVariables = 1

    elif haruspexName == "order":
        haruspex = vtkOrderStatistics()
        numVariables = 1

    elif haruspexName == "correlative":
        haruspex = vtkCorrelativeStatistics()
        numVariables = 2

    elif haruspexName == "contingency":
        haruspex = vtkContingencyStatistics()
        numVariables = 2

    elif haruspexName == "multicorrelative":
        haruspex = vtkMultiCorrelativeStatistics()
        numVariables = 3

    elif haruspexName == "PCA":
        haruspex = vtkPCAStatistics()
        numVariables = 3

    else:
        print "ERROR: Invalid statistics engine:", haruspexName
        sys.exit( 1 )

    if verbosity > 0:
        print "# Instantiated a", haruspex.GetClassName(), "object"
        print "  Number of variables:", numVariables
        print

    return [ haruspex, numVariables ]
############################################################

############################################################
# Usage function
def Usage( outModelPrefix, outDataName, verbosity ):
    print "Usage:"
    print "\t -h               Help: print this message and exit"
    print "\t -d <filename>    CSV input data file"
    print "\t -e <haruspex>    Type of statistics engine"
    print "\t [-s <filename> ] CSV output model (statistics) file prefix. Default:",outModelPrefix
    print "\t [-a <filename> ] CSV output data (annotated) file. Default:",outDataName
    print "\t [-v]             Increase verbosity (0 = silent). Default:",verbosity
    sys.exit( 1 )
############################################################

############################################################
# Read input CSV data as input port
def ReadInData( inDataName, verbosity ):
    if verbosity > 0:
        print "# Reading input data:"

    # Set CSV reader parameters
    inData = vtkDelimitedTextReader()
    inData.SetFieldDelimiterCharacters(",")
    inData.SetHaveHeaders(True)
    inData.SetDetectNumericColumns(True)
    inData.SetFileName(inDataName)
    inData.Update()

    if verbosity > 0:
        T = inData.GetOutput()
        print "  Number of columns:", T.GetNumberOfColumns()
        print "  Number of rows:", T.GetNumberOfRows()
        print
        if verbosity > 1:
            print "# Input data:"
            inData.GetOutput().Dump( 10 )
            print
    
    return inData
############################################################

############################################################
# Write haruspex output data
def WriteOutData( haruspex, outDataName, verbosity ):
    if verbosity > 0:
        print "# Saving output (annotated) data:"

    # Set CSV writer parameters
    outDataWriter = vtkDelimitedTextWriter()
    outDataWriter.SetFieldDelimiter(",")
    outDataWriter.SetFileName( outDataName )
    outDataWriter.SetInputConnection( haruspex.GetOutputPort( 0 ) )
    outDataWriter.Update()

    if verbosity > 0:
        print "  Wrote", outDataName
        print
############################################################

############################################################
# Write haruspex output model
def WriteOutModel( haruspex, outModelPrefix, verbosity ):
    if verbosity > 0:
        print "# Saving output model (statistics):"
        
    # Set CSV writer parameters
    outModelWriter = vtkDelimitedTextWriter()
    outModelWriter.SetFieldDelimiter(",")

    # Get output model type to select appropriate write scheme
    outModelType = haruspex.GetOutputDataObject( 1 ).GetClassName()
    if verbosity > 0:
        print "  Output model is a", outModelType

    # Select write scheme depending on output model type
    if outModelType == "vtkTable":
        # Straightforward CSV file dump of a vtkTable
        outModelName = outModelPrefix + ".csv"
        outModelWriter.SetFileName( outModelName )
        outModelWriter.SetInputConnection( haruspex.GetOutputPort( 1 ) )
        outModelWriter.Update()

        if verbosity > 0:
            print "  Wrote", outModelPrefix
            if verbosity > 1:
                haruspex.GetOutput( 1 ).Dump( 10 )

    elif outModelType == "vtkMultiBlockDataSet":
        # Must iterate over all blocks of the vtkMultiBlockDataSet
        outModel = haruspex.GetOutputDataObject( 1 )
        n = outModel.GetNumberOfBlocks()
        for i in range( 0, n ):
            outModelName = outModelPrefix + "-" + str( i )+ ".csv"
            outModelWriter.SetFileName( outModelName )
            table = outModel.GetBlock( i )
            outModelWriter.SetInput( table )
            outModelWriter.Update()
            
            if verbosity > 0:
                print "  Wrote", outModelName
                if verbosity > 1:
                    table.Dump( 10 )

    print
############################################################

############################################################
# Calculate statistics
def CalculateStatistics( inData, haruspex, numVariables, verbosity ):
    if verbosity > 0:
        print "# Calculating statistics:"

    # Output port of data reader becomes input connection of haruspex
    haruspex.AddInputConnection(inData.GetOutputPort() )

    # Get the output table of the data reader
    table = inData.GetOutput()

    # Generate list of columns of interest, depending on number of variables
    if numVariables == 1:
        # Univariate case: one request for each columns
        for i in range( 0, table.GetNumberOfColumns() ):
            colName = table.GetColumnName( i )
            if verbosity > 0:
                print "  Requesting column",colName
            haruspex.AddColumn(colName)

    elif numVariables == 2:
        # Bivariate case: generate all possible pairs
        for i in range( 0, table.GetNumberOfColumns() ):
            colNameX = table.GetColumnName( i )
            for j in range( i+1,table.GetNumberOfColumns() ):
                colNameY = table.GetColumnName( j )
                if verbosity > 0:
                    print "  Requesting column pair",colNameX,colNameY
                haruspex.AddColumnPair(colNameX,colNameY)

    elif numVariables == 3:
        # Multivariate case: generate single request containing all columns
        for i in range( 0, table.GetNumberOfColumns() ):
            colName = table.GetColumnName( i )
            haruspex.SetColumnStatus( colName, 1 )
            if verbosity > 0:
                print "  Adding column", colName, "to the request"

    else:
        print "ERROR: Unsupported case: number of variables = ",numVariables
        sys.exit( 1 )

    # Complete column selection request
    haruspex.RequestSelectedColumns()
    
    # Calculate statistics with Learn, Derive, and Assess options turned on (Test is left out for now)
    haruspex.SetLearnOption(True)
    haruspex.SetDeriveOption(True)
    haruspex.SetAssessOption(True)
    haruspex.SetTestOption(False)
    haruspex.Update()

    print "  Done"
    print
############################################################

############################################################
# Main function
def main():
    # Parse command line
    [ inDataName, haruspexName, outModelPrefix, outDataName, verbosity ] = ParseCommandLine()

    # Verify that haruspex name makes sense and if so instantiate accordingly
    [ haruspex, numVariables ] = InstantiateStatistics( haruspexName, verbosity )

    # Get input data port
    inData = ReadInData( inDataName, verbosity )

    # Calculate statistics
    CalculateStatistics( inData, haruspex, numVariables, verbosity )

    # Save output (annotated) data
    WriteOutData( haruspex, outDataName, verbosity )

    # Save output model (statistics)
    WriteOutModel( haruspex, outModelPrefix, verbosity )
############################################################

############################################################
if __name__ == "__main__":
    main()
############################################################
