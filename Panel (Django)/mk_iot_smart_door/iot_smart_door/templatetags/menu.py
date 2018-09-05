from django import template
from django.urls import resolve

register = template.Library()


@register.simple_tag(takes_context=True)
def url_ogren(context):
  request = context['request']

  return str(request.path_info)
