#include <QtDebug>
#include "smdescription.h"

SMDescription::SMDescription()
{    
}

void SMDescription::clear()
{
	stateList.clear();
	transitionList.clear();
	title.clear();
	initState.clear();
	errorState.clear();
}

void SMDescription::addState(t_state state)
{
	bool _stateAlreadyExists = false;
	int _existingStateIndex = 0;

	for( _existingStateIndex = 0; _existingStateIndex < stateList.size(); _existingStateIndex++) {
		if( stateList[_existingStateIndex].name == state.name )
		{
			 _stateAlreadyExists = true;
		 }
	}

	// If the state has not been saved yet
	if( !_stateAlreadyExists )
	{
        // Save it in the list
        stateList.append(state);
    }
	else
	{
		// If we have additionnal informations concerning this state, we save them
		if( state.entryFct != "" )
			stateList[_existingStateIndex].entryFct = state.entryFct;
		if( state.duringFct != "" )
			stateList[_existingStateIndex].duringFct = state.duringFct;
		if( state.isInit != FALSE )
			stateList[_existingStateIndex].isInit = state.isInit;
		if( state.isError != FALSE )
			stateList[_existingStateIndex].isError = state.isError;
	}
}

void SMDescription::addTitle(QString title)
{
    this->title = title;
	formatTitle();
}

void SMDescription::addTransition(t_transition transition)
{
    transitionList.append(transition);
}

void SMDescription::debugTransitionList()
{
    for( int i=0; i<transitionList.size(); i++)
    {
        debugTransition(transitionList.at(i));
    }
}

void SMDescription::debugTransition(t_transition transition)
{
    qDebug() << transition.fromState << " -> " << transition.toState << endl
             << "\t" << transition.condition << " / " << transition.action;
}

// Format Title in lower case and whith whitespaces replaced by '_'
void SMDescription::formatTitle()
{
	if( ! title.isEmpty() )
	{
		formatedTitle = title.toLower();

		// Replace multiple spaces (or \t, \n, etc.) with a simple space
		formatedTitle = formatedTitle.simplified();

		formatedTitle.replace(' ','_');
	}
}
