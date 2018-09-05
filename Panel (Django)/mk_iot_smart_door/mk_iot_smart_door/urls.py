from django.contrib import admin
from django.urls import path

from iot_smart_door.views import home, cikis, panel_dashboard, panel_cards, panel_doors, panel_personnels, api_add_door, api_delete_door, api_get_door, api_update_door, api_control

urlpatterns = [
    path('admin/', admin.site.urls),
    path('', home),
    path('logout/', cikis),
    path('panel/dashboard/', panel_dashboard),
    path('panel/doors/', panel_doors),
    path('panel/cards/', panel_cards),
    path('panel/personnels/', panel_personnels),

    # API's
    path('api/control/', api_control),

    path('api/get-door/', api_get_door),
    path('api/add-door/', api_add_door),
    path('api/delete-door/', api_delete_door),
    path('api/update-door/', api_update_door),
]
