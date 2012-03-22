/*========================================================================
  Midas Platform

      http://midasplatform.org

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

// Increase version number every time the binary should be recompiled
// for all architectures
#define VERSION "1.0"

// SimpleITK includes
#include <SimpleITK.h>

// ITK Includes
#include "metaCommand.h"

// Simp
namespace sitk = itk::simple;

int main(int argc, char* argv[])
{

  // Metacommand setup
  MetaCommand command;
  command.SetName("Thumbnailer");
  command.SetVersion(VERSION);
  command.SetAuthor("Patrick Reynolds");
  command.SetDescription("Generate a thumbnail from a 2D or 3D image");
  command.AddField("InputFile", "Input File", MetaCommand::STRING);
  command.AddField("OutputFile", "Output Thumbnail", MetaCommand::STRING);
  command.SetOption("Loud", "l", false, "Output with extreme verbosity.",
                    MetaCommand::FLAG);
  command.SetOptionLongTag("Loud", "loud");
  if(!command.Parse(argc, argv))
    {
    return 1;
    }

  std::string inputImage = command.GetValueAsString("InputFile");
  std::string outputImage = command.GetValueAsString("OutputFile");
  bool loud = command.GetValueAsBool("Loud");

  sitk::ImageFileReader                reader;
  sitk::RescaleIntensityImageFilter    rescaler;
  sitk::ImageFileWriter                writer;
  sitk::ExtractImageFilter             extractor;
  sitk::CastImageFilter                caster;
  sitk::ScalarToRGBColormapImageFilter colorizer;

  sitk::Image image;
  sitk::Image slice;
  sitk::Image rescaledImage;
  sitk::Image castImage;
  sitk::Image colorImage;

  std::vector<unsigned int> size;
  std::vector<int>          index;

  reader.SetFileName(inputImage);
  image = reader.Execute();

  unsigned int dimension = image.GetDimension();
  if( dimension == 2 )
    {
    if( loud )
      {
      std::cout << "-- The input image is 2D." << std::endl;
      }
    slice = image;
    }
  else if( dimension == 3 )
    {
    if( loud )
      {
      std::cout << "-- The input image is 3D." << std::endl;
      }
    size = image.GetSize();
    int middleOfTheImageInZ = size[2] / 2;
    if( loud )
      {
      std::cout << "-- The medial slice is " << middleOfTheImageInZ << "."
                << std::endl;
      }
    index.push_back( 0 );
    index.push_back( 0 );
    index.push_back( middleOfTheImageInZ );
    size[2] = 0;
    extractor.SetSize( size );
    extractor.SetIndex( index );
    if( loud )
      {
      std::cout << "-- Extracting the medial slice." << std::endl;
      }
    slice = extractor.Execute( image );
    }
  else
    {
    std::cerr << "Unable to handle images with " 
              << dimension << " dimensions." << std::endl;
    return -1;
    }

  if( loud )
    {
    std::cout << "-- Rescaling image intensities between 0 and 255."
              << std::endl;
    }
  rescaler.SetOutputMinimum( 0 );
  rescaler.SetOutputMaximum( 255 );
  rescaledImage = rescaler.Execute( slice );

  if( loud )
    {
    std::cout << "-- Cast the image into a uint_8 (unsigned char)."
              << std::endl;
    }
  caster.SetOutputPixelType( sitk::sitkUInt8 );
  castImage = caster.Execute( rescaledImage );

  if( loud )
    {
    std::cout << "-- Convert the image into RGB for JPEG ouput. This "
              << "application does not require JPEGs, but JPEGs require RGB "
              << "for ITK to output them correctly." << std::endl;
    }
  colorizer.SetColormap(  sitk::ScalarToRGBColormapImageFilter::Grey );
  colorImage = colorizer.Execute( castImage );

  if( loud )
    {
    std::cout << "-- Output image statistics" << std::endl;
    std::cout << "--- Number of Dimensions: " << colorImage.GetDimension()
              << std::endl;
    std::cout << "--- Width: " << colorImage.GetWidth() << std::endl;
    std::cout << "--- Height: " << colorImage.GetHeight() << std::endl;
    std::cout << "--- Depth (should be 0): " << colorImage.GetDepth()
              << std::endl;
    std::cout << "-- Writing the output to disk." << std::endl;
    }
  writer.SetFileName( outputImage );
  writer.Execute( colorImage );

  return 0;
}
