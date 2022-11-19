package isep.model.mapper;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.naming.NameNotFoundException;
import isep.controller.App;
import isep.model.AgriculturalParcel;
import isep.model.Company;
import isep.model.IrrigationPlan;
import isep.model.ParcelIrrigationWrapper;
import isep.model.Regularity;
import isep.model.store.AgriculturalParcelStore;
import isep.shared.Hour;
import isep.shared.exceptions.InvalidFileFormatException;
import isep.shared.exceptions.InvalidHourFormatException;

/**
 * The IrrigationPlanMapper class takes a list as an input and outputs an IrrigationPlan object.
 *
 * @author Ricardo Moreira <1211285@isep.ipp.pt>
 */
public class IrrigationPlanMapper {
  private IrrigationPlanMapper() {}

  public static IrrigationPlan toPlan(List<String> data, Company company)
      throws InvalidHourFormatException, InvalidFileFormatException, NameNotFoundException {
    AgriculturalParcelStore parcelStore = company.getAgriculturalParcelStore();

    List<Hour> hours = new ArrayList<>();
    Map<AgriculturalParcel, ParcelIrrigationWrapper> parcels = new HashMap<>();

    // header contains hours
    String[] header = data.get(0).split(",");
    for (int i = 1; i < header.length; i++) {
      hours.add(new Hour(header[i]));
    }

    // the following lines contain the irrigation plan for a specific parcel
    for (int i = 1; i < data.size(); i++) {
      String[] line = data.get(i).split(",");
      if (line.length != 3)
        throw new InvalidFileFormatException(String.format("File is not ok, line %d", i + 1));

      String parcelDesignation = line[0];
      String durationStr = line[1];
      String regularity = line[2];

      AgriculturalParcel foundParcel = parcelStore.findParcelByDesignation(parcelDesignation);
      if (foundParcel == null)
        throw new NameNotFoundException(
            String.format("Parcel %s not found in store", parcelDesignation));

      int duration = Integer.parseInt(durationStr);

      Regularity method = null;

      switch (regularity) {
        case "todos":
          method = (day) -> (true);
          break;
        case "pares":
          method = (day) -> (day % 2 == 0);
          break;
        case "impares":
          method = (day) -> (day % 2 == 1);
          break;
        default:
          throw new InvalidFileFormatException(
              String.format("Invalid regularity %s in line %d", regularity, i + 1));
      }

      ParcelIrrigationWrapper wrapper = new ParcelIrrigationWrapper(duration, method);

      parcels.put(foundParcel, wrapper);
    }

    IrrigationPlan irrigationPlan = new IrrigationPlan(hours, parcels);

    return irrigationPlan;
  }
}
