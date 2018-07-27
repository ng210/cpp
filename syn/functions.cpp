#include "synapp.h"
#include "synsequence.h"
#include "utils/file.h"
#include "player/channel.h"

NS_SYN_USE
NS_PLAYER_USE

//PArray* SynApp::importSequencesFromXM(const char* file) {
//	XmLoader xmLoader(playerAdapter_, synthAdapter_);
//	PArray* sequences = NULL;
//	if (xmLoader.load(file) == 0) {
//		// xm loaded into structures
//		char buffer[512];
//		sprintf_s(buffer, 512, "Pattern count: %d\n", xmLoader.patterns().length());
//		log_.append(buffer, NS_FW_BASE::strlen(buffer));
//		// convert xm data to sequences
//		sequences = xmLoader.convert();
//		sprintf_s(buffer, 512, "Sequence count: %d\n", sequences->length());
//		log_.append(buffer, NS_FW_BASE::strlen(buffer));
//
//		SynSequence* sequence = (SynSequence*)sequences->getAt(0);
//		sequence->print(playerAdapter_, &log_);
//		for (UINT8 i = 1; i < sequences->length(); i++) {
//			sequence = (SynSequence*)sequences->getAt(i);
//			sequence->print(synthAdapter_, &log_);
//		}
//		File::write("syn.seq.txt", &log_);
//	}
//	return sequences;
//}

static AbstractChannel* createChannel(void) {
	return NEW_(NS_PLAYER::Channel);
}

#define MYCONTROLID 0x1234
//void SynApp::prepareViews() {
//	// create multichartview
//	multiChartView_ = NEW_(MultiChartView, this, MYCONTROLID);
//
//	CHARTCHANNELINFO* channels = MALLOC(CHARTCHANNELINFO, 4);
//	int ci = 0;
//	channels[ci].min = 0.0f; channels[ci].max = 100.0f; channels[ci].color = 0xF00000;
//	channels[ci].paintMode = CHART_PAINTMODE_BAR; channels[ci].label = "Integer";
//	ci++;
//
//	multiChartView_->setDataSource(dataSeries_, ci, channels);
//	//multiChartView_->configuration().customDraw[0] = &SynApp::drawBox;
//	RECT rect;
//	SYSPR(GetClientRect(hWnd_, &rect));
//	SYSPR(SetWindowPos(multiChartView_->hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
//}
//
//void SynApp::createSeries(PArray* sequence) {
//	// create a dataseries object for each sequence
//	if (series_ != NULL) {
//		ARRAY_FOREACH(series_, DEL_((SeqSeries*)value));
//		DEL_(series_);
//	}
//	series_ = NEW_(PArray);
//	for (UINT32 i = 1; i < sequences_->length(); i++) {
//		Sequence* sequence = (Sequence*)sequences_->getAt(i);
//		SeqSeries* series = NEW_(SeqSeries, sequence, synthAdapter_);
//		series_->add(series);
//	}
//
//}
//
//void SynApp::preparePlayer() {
//	DEL_(synthAdapter_);
//	DEL_(player_);
//	Player::createChannel = createChannel;
//	player_ = NEW_(Player);
//	playerAdapter_ = (PlayerAdapter*)player_->adapters()->getAt(0);
//	synthAdapter_ = NEW_(SynthAdapter);
//	player_->adapters()->add(synthAdapter_);
//}
//
//PArray* SynApp::createSequences() {
//	PArray* sequences = NEW_(PArray);
//	Sequence* sequence;
//	Frame* frame;
//	// create main sequence
//	sequences->add(sequence = NEW_(Sequence));
//	frame = sequence->addFrame(0);
//	IAdapter* adapter = playerAdapter_;
//	frame->addCommand(adapter->createCommand(Player_Cmd_create, 1, 0));
//	frame->addCommand(adapter->createCommand(Player_Cmd_setTempo, 720.0f));
//	frame->addCommand(adapter->createCommand(Player_Cmd_assign, 1, 1, Player_Flg_Active | 1));
//	frame = sequence->addFrame(32);
//	frame->addCommand(adapter->createCommand(Player_Cmd_assign, 1, 2, Player_Flg_Active | 1));
//	frame = sequence->addFrame(64);
//	frame->addCommand(IAdapter::createEndCommand());
//	// create #1
//	sequences->add(sequence = NEW_(Sequence));
//	frame = sequence->addFrame( 0);
//	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_prgChange, 3));
//	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn,  F(3), 128));
//	frame = sequence->addFrame( 2);
//	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, F(3)));
//	frame = sequence->addFrame( 4);
//	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn,  Gis(3), 160));
//	frame = sequence->addFrame( 6);
//	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, Gis(3)));
//	frame = sequence->addFrame( 8);
//	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn,  C(4), 160));
//	frame = sequence->addFrame(10);
//	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(4)));
//	frame = sequence->addFrame(12);
//	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn,  Gis(3), 128));
//	frame = sequence->addFrame(14);
//	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, Gis(3)));
//	frame = sequence->addFrame(16);
//	frame->addCommand(adapter->createEndCommand());
//	// create #2
//	sequences->add(sequence = NEW_(Sequence));
//	frame = sequence->addFrame(0);
//	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_prgChange, 3));
//	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn,  Dis(3), 128));
//	frame = sequence->addFrame(2);
//	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, Dis(3)));
//	frame = sequence->addFrame(4);
//	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn,  G(3), 160));
//	frame = sequence->addFrame(6);
//	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, G(3)));
//	frame = sequence->addFrame(8);
//	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn,  Ais(3), 128));
//	frame = sequence->addFrame(10);
//	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, Ais(3)));
//	frame = sequence->addFrame(12);
//	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn,  G(3), 96));
//	frame = sequence->addFrame(14);
//	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, G(3)));
//	frame = sequence->addFrame(16);
//	frame->addCommand(adapter->createEndCommand());
//	return sequences;
//}