# e-Ink
## Adventures with e-INK (phoroelectric) displays
This is a TI Code Composer Studio project using the MSP430 Launchpad (original model or new model) fitted with a MSP430G2553 to operate a WaveShare 1.54" e-Paper

This project is found in the yearbook under the article "Adventures with e-Ink"

## Usage
1. Git clone to the current CCS workspace
2. Import into CCS (Since this product is Eclipse-based, either Import/Open usually works)
3. Build
4. Connect board and display as shown in the yearbook
5. Debug and Run

## Preparing displayed images
Note that to change the displayed images requires the following items
1. LCDImager - A bespoke Delphi application designed to solve the problem of turning pixels to C arrays (or assembler arrays) Courtesy copy included in repo
2. Adobe Photoshop CC - to resize the images smoothly and to handle the iff file format that tool.anides.de exports
3. Access to tool.anides.de (Retro Image Tool) to convert a colour or grayscale image into decent 1bpp art using various algorithms to get the best results
