
/***************************************************************************
 *  iface_field_wrapper.h - Wrapper for a Fawkes interface field for XABSL
 *
 *  Created: Thu Aug 07 18:50:52 2008
 *  Copyright  2006-2008  Tim Niemueller [www.niemueller.de]
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#ifndef _PLUGINS_XABSL_IFACE_FIELD_WRAPPER_H_
#define _PLUGINS_XABSL_IFACE_FIELD_WRAPPER_H_

#include <interface/field_pointer.h>
#include <XabslEngine/XabslSymbols.h>

/** Interface field wrapper for Xabsl.
 * This wraps a field of an iterface in a Xabsl function provider such that
 * the field can be used as input and/or output symbol in Xabsl.
 * This class does an implicit conversion of types. For instance in a
 * BlackBoard interface floats are stored, while Xabsl requires doubles. With
 * an explicit casting conversion code is generated by the compiler to make it
 * work.
 * @author Tim Niemueller.
 */
template <typename XabslType, typename FieldType>
  class XabslInterfaceFieldWrapper : public xabsl::FunctionProvider
{
 public:
  /** Constructor.
   * @param type value type of the field
   * @param name name of the field
   * @param value pointer to the value of the field
   */
  XabslInterfaceFieldWrapper(fawkes::Interface::interface_fieldtype_t type,
			     const char *name, FieldType *value)
  {
    pointer_ = new fawkes::InterfaceFieldPointer<FieldType>(type, name, value);
  }

  /** Destructor. */
  ~XabslInterfaceFieldWrapper()
  {
    delete pointer_;
  }

  /** Get name of the field.
   * @return name of the field.
   */
  const char * get_name() const
  {
    return pointer_->get_name();
  }

  /** Get type of the field.
   * @return type of the field.
   */
  fawkes::Interface::interface_fieldtype_t  get_type() const
  {
    return pointer_->get_type();
  }

  /** Get current value.
   * @return current value in the Xabsl type
   */
  XabslType get_value() const
  {
    return (XabslType)pointer_->get_value();
  }

  /** Set new value.
   * @param new_value new value, converted to field type
   */
  void set_value(XabslType new_value)
  {
    pointer_->set_value((FieldType)new_value);
  }

 private:
  fawkes::InterfaceFieldPointer<FieldType> *pointer_;
};

#endif
