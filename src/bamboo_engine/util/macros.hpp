//
// Created by Leon Suchy on 06.08.20.
//

#ifndef BAMBOOENGINE_MACROS_HPP
#define BAMBOOENGINE_MACROS_HPP

#define BBGE_NO_COPIES(clazz) \
clazz(clazz&) = delete; \
clazz& operator=(clazz&) = delete;

#define BBGE_NO_MOVES(clazz) \
clazz(clazz&&) = delete; \
clazz& operator=(clazz&&) = delete;

#endif //BAMBOOENGINE_MACROS_HPP
