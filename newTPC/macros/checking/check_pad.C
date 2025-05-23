void check_pad()
{
  auto tpc = new NewTPC();
  tpc -> AddPar("./par_NewTPC.conf");
  tpc -> Init();

  auto padplane = (NewTPCHoneyCombPad*) tpc -> GetPadPlane();
  padplane -> Print();
  padplane -> Clear();

  // auto hist_padplane = padplane -> GetHist("0");
  auto numPads = padplane -> GetNumPads();
  auto hist_padplane = padplane -> GetHist();

  for (auto iPad=0; iPad<numPads; ++iPad) {
    auto pad = padplane -> GetPad(iPad);
    auto bin = hist_padplane -> FindBin(pad -> GetI(), pad -> GetJ());
    hist_padplane -> SetBinContent(bin,iPad);
  }

  auto cvs = padplane -> GetCanvas();
  // hist_padplane -> SetName("PadPlane");
  // hist_padplane -> SetTitle("PadPlane");
  padplane -> FindPadID(0,0);
  gStyle -> SetPalette(kBird);
  hist_padplane -> Draw("colz");
  //hist_padplane -> Draw("text");
  padplane -> DrawFrame();

  padplane -> PadPositionChecker();

}
