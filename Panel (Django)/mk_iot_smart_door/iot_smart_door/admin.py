from django.contrib import admin
from django.utils.safestring import mark_safe

from iot_smart_door.models import Cards, Doors, Personnels, Logs


class DoorsAdmin(admin.ModelAdmin):
    list_display = ['name']
    list_display_links = list_display
    search_fields = list_display


class CardsAdmin(admin.ModelAdmin):
    list_display = ('identity', 'personnel', 'get_authorized_doors', 'get_unauthorized_doors', 'get_banned_doors', 'banned')
    list_display_links = list_display

    def get_authorized_doors(self, obj):
        return mark_safe("<br/>".join([p.name for p in obj.authorized_doors.all()]))

    get_authorized_doors.short_description = "İzinli Kapılar"

    def get_unauthorized_doors(self, obj):
        return mark_safe("<br/>".join([p.name for p in obj.unauthorized_doors.all()]))

    get_unauthorized_doors.short_description = "İzinsiz Kapılar"

    def get_banned_doors(self, obj):
        return mark_safe("<br/>".join([p.name for p in obj.banned_doors.all()]))

    get_banned_doors.short_description = "Yasaklı Kapılar"

    class Meta:
        model = Cards


class PersonnelsAdmin(admin.ModelAdmin):
    list_display = ['first_name', 'last_name', 'email', 'phone_number', 'is_visible']
    list_display_links = ['first_name', 'last_name', 'email', 'phone_number']
    list_editable = ['is_visible']

    class Meta:
        model = Personnels


class LogsAdmin(admin.ModelAdmin):
    list_display = ["door", "card", "personnel", "status", "message", "reason", "create_at"]
    list_display_links = list_display


admin.site.register(Doors, DoorsAdmin)
admin.site.register(Cards, CardsAdmin)
admin.site.register(Personnels, PersonnelsAdmin)
admin.site.register(Logs, LogsAdmin)
