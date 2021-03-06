/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/


#include "lispirep.h"

#include "irep.h"
#include "lispexpr.h"

void lisp2irep(const lispexprt &src, irept &dest)
{
  dest.make_nil();

  if(src.type!=lispexprt::List || src.empty())
    return;

  dest.id(src[0].value);

  for(std::size_t i=1; i<src.size(); i++)
    if(!src[i].is_nil())
    {
      const std::string &name=src[i].value;
      i++;

      if(i<src.size())
      {
        irept sub;
        lisp2irep(src[i], sub);

        if(name=="")
          dest.move_to_sub(sub);
        else
          dest.move_to_named_sub(name, sub);
      }
    }
}

void irep2lisp(const irept &src, lispexprt &dest)
{
  dest.clear();
  dest.value="";
  dest.type=lispexprt::List;

  dest.reserve(2+2*src.get_sub().size()
                +2*src.get_named_sub().size()
                +2*src.get_comments().size());

  lispexprt id;
  id.type=lispexprt::String;
  id.value=src.id_string();
  dest.push_back(id);

  // reserve objects for extra performance

  forall_irep(it, src.get_sub())
  {
    lispexprt name;
    name.type=lispexprt::String;
    name.value="";
    dest.push_back(name);

    lispexprt sub;

    irep2lisp(*it, sub);
    dest.push_back(sub);
  }

  forall_named_irep(it, src.get_named_sub())
  {
    lispexprt name;
    name.type=lispexprt::String;
    name.value=name2string(it->first);
    dest.push_back(name);

    lispexprt sub;

    irep2lisp(it->second, sub);
    dest.push_back(sub);
  }

  forall_named_irep(it, src.get_comments())
  {
    lispexprt name;
    name.type=lispexprt::String;
    name.value=name2string(it->first);
    dest.push_back(name);

    lispexprt sub;

    irep2lisp(it->second, sub);
    dest.push_back(sub);
  }

  lispexprt nil;
  nil.type=lispexprt::Symbol;
  nil.value="nil";

  dest.push_back(nil);
}
