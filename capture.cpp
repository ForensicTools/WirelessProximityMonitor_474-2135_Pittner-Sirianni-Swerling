#include "capture.h"


CaptureText::CaptureText()
{
  set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  add(m_TextView);
  insert_text();

  show_all_children();
}

CaptureText::~CaptureText()
{
}

void CaptureText::insert_text(char* packet)
{
  Glib::RefPtr<Gtk::TextBuffer> refTextBuffer = m_TextView.get_buffer();

  Gtk::TextBuffer::iterator iter = refTextBuffer->get_iter_at_offset(0);
  refTextBuffer->insert(iter, packet);
}