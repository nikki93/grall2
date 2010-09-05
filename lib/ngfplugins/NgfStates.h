/*
 * =====================================================================================
 *
 *       Filename:  NgfStates.h
 *
 *    Description:  State system for GameObjects. Inspired by 'States' in UnrealScript.
 *
 *        Created:  11/07/2009 05:57:54 PM
 *       Compiler:  gcc
 *
 *         Author:  Nikhilesh S (nikki)
 *
 * =====================================================================================
 */

#include <vector>
#include <map>

//Define the common state interface with these macros.
#define NGF_STATES_INTERFACE_BEGIN(classname)                                             \
        struct __State;                                                                \
        friend class __State;                                                          \
        struct __StateBase                                                             \
        {                                                                              \
                classname *mObj;                                                       \
                __StateBase *mPrevState, *mNextState;                                  \
                unsigned int mIndex;                                                   \
                                                                                       \
                __StateBase()                                                          \
                    : mPrevState(0),                                                   \
                      mNextState(0)                                                    \
                {                                                                      \
                }                                                                      \
                                                                                       \
                inline unsigned int getIndex() { return mIndex; }                      \
                                                                                       \
                virtual void enter() {}                                                \
                virtual void exit() {}                                                 \
        };                                                                             \
        struct __State : public __StateBase
#define NGF_STATES_INTERFACE_END                                                          \
        *mCurrState, *__mNextState;                                                    \
        std::vector<__State *> __mStateStack;                                          \
        std::map<unsigned int, __State *> __mStateMap;

//Define each state with this macro.
#define NGF_STATES_STATE_BEGIN(state)                                                     \
        struct state;                                                                  \
        state *__mState_##state;                                                       \
        enum { St_##state = __LINE__ };                                                \
        struct state : public __State
#define NGF_STATES_STATE_END(state)                                                       \
        mCurrStateAuto_##state;

//Initialise the FSM and states.
#define NGF_STATES_INIT()                                                                 \
        do                                                                             \
        {                                                                              \
            mCurrState = 0;                                                            \
            __mNextState = 0;                                                          \
        } while (0)
#define NGF_STATES_INIT_STATE(state)                                                      \
        do                                                                             \
        {                                                                              \
            __mState_##state = &mCurrStateAuto_##state;                                \
            __mState_##state->mObj = this;                                             \
            __mState_##state->mIndex = St_##state;                                     \
            __mStateMap[St_##state] = __mState_##state;                                \
        } while (0)

//Update the FSM. Call per 'tick'.      
#define NGF_STATES_UPDATE()                                                               \
        do                                                                             \
        {                                                                              \
            if (__mNextState)                                                          \
            {                                                                          \
                __NGF_STATES_SWITCH_STATE(__mNextState);                                  \
                __mNextState = 0;                                                      \
            }                                                                          \
        } while (0)

//Switches immediately (call this from containing class).
#define NGF_STATES_SWITCH_STATE(state)                                                    \
        do                                                                             \
        {                                                                              \
            __NGF_STATES_SWITCH_STATE(__mState_##state);                               \
        } while (0)

//Switches on next update (call this from the state code).
#define NGF_STATES_CONTAINER_SWITCH(state)                                                \
        mObj->__mNextState = mObj->__mState_##state

//Check whether current state is given one.
#define NGF_STATES_CURRENT_STATE(state)                                                \
        (mCurrState == __mState_##state)

//Push given state onto the state stack. Will start running this state, till popped.
#define NGF_STATES_PUSH_STATE(state)                                                      \
        do                                                                             \
        {                                                                              \
            if(mCurrState && __mStateStack.empty())                                                  \
                __mStateStack.push_back(mCurrState);                                        \
            __mStateStack.push_back(__mState_##state);                                      \
            NGF_STATES_SWITCH_STATE(state);                                            \
        } while (0)

//Remove the state on top of the stack. Will exit the current state, and run the one
//before the top on stack. If the stack is empty after popping, will not exit current
//state.
#define NGF_STATES_POP_STATE()                                                            \
        do                                                                             \
        {                                                                              \
            if (!__mStateStack.empty())                                                \
            {                                                                          \
                __mStateStack.pop_back();                                                       \
                __NGF_STATES_SWITCH_STATE(__mStateStack.back());                        \
            }                                                                          \
        } while (0)

//Clear the state stack.
#define NGF_STATES_CLEAR_STACK()                                                       \
        (__mStateStack.clear())

//Push given state onto the state stack. Will start running this state, till popped.
#define NGF_STATES_CONTAINER_PUSH_STATE(state)                                            \
        do                                                                             \
        {                                                                              \
            if(mObj->__mStateStack.empty())                                                  \
                mObj->__mStateStack.push_back(mObj->mCurrState);                                        \
            mObj->__mStateStack.push_back(mObj->__mState_##state);                                      \
            NGF_STATES_CONTAINER_SWITCH(state);                                        \
        } while (0)

//Remove the state on top of the stack. Will exit the current state, and run the one
//before the top on stack. If the stack is empty after popping, will not exit current
//state.
#define NGF_STATES_CONTAINER_POP_STATE(state)                                          \
        do                                                                             \
        {                                                                              \
            if (!mObj->__mStateStack.empty())                                                \
            {                                                                          \
                mObj->__mStateStack.pop_back();                                                       \
                mObj->__mNextState = mObj->__mStateStack.back();                        \
            }                                                                          \
        } while (0)

//Get the state pointer from index.
#define NGF_STATES_GET_STATE_FROM_INDEX(ind)                                           \
        (__mStateMap[ind])

//Internal switch method (takes state pointer name).
#define __NGF_STATES_SWITCH_STATE(ptr1)                                                    \
        do                                                                             \
        {                                                                              \
            __State *ptr = ptr1; \
            ptr->mPrevState = mCurrState;                                              \
            __mNextState = 0;                                                          \
                                                                                       \
            if (mCurrState)                                                            \
            {                                                                          \
                mCurrState->mNextState = ptr;                                          \
                mCurrState->exit();                                                    \
                mCurrState->mNextState = 0;                                            \
            }                                                                          \
                                                                                       \
            if (__mNextState)                                                          \
            {                                                                          \
                mCurrState = __mNextState;                                             \
                __mNextState = 0;                                                      \
            }                                                                          \
            else                                                                       \
                mCurrState = ptr;                                                      \
                                                                                       \
            mCurrState->enter();                                                       \
            mCurrState->mPrevState = 0;                                                \
        } while (0)

